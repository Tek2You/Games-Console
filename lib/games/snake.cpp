﻿/* snake.cpp : Snake game library
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this library.  If not, see http://www.gnu.org/licenses/.
 */

#include <avr/eeprom.h>
#include <avr/wdt.h>
#include <operators.h>
#include <snake.h>

using namespace event_handling;

uint16_t Snake::EE_highscore_ EEMEM = 0;
uint16_t Snake::highscore_ = eeprom_read_word(&EE_highscore_);
static const uint16_t periods[] PROGMEM = { 600, 500, 400, 275, 180 };

Snake::Snake(Display* display) : Game(display, SNAKE), body_(display->rows() * display->cols()), timer_(nullptr) {
	// set speed for case that it wouldnt be done
	Snake::setSpeed(2);
	dir_ = START;
}

Snake::~Snake() {}

void Snake::start(Event* event) {
	// Create initial Snake
	body_ << SmartPos(1, 7);
	body_ << SmartPos(2, 7);
	body_ << SmartPos(3, 7);  // head

	// Initial eat
	eat_pos_ = SmartPos(5, 7);

	// setup directions
	dir_ = START;

	// Setup timer for moving
	timer_ = new Timer();
	timer_->setInterval(pgm_read_word(&periods[speed_]));
	event->addTrigger(timer_);

	// print display first time
	render();
}

// stores intervals for diferent speeds in progmem

void Snake::setSpeed(const byte v) {
	speed_ = v;
}

void Snake::resetHighscore() {
	// reset highscore
	highscore_ = 0;
	// take over to eeprom
	eeprom_write_word(&EE_highscore_, highscore_);
}

bool Snake::onButtonChange(Event* event) {
	// ensure that there is not ability to make a rotation by 180°
	if (event->buttonPressed(BUTTON_UP)) {
		if (!(dir_ != Snake::DOWN && dir_ != Snake::UP))
			return false;
		dir_ = Snake::UP;
	} else if (event->buttonPressed(BUTTON_RIGHT)) {
		if (!(dir_ != Snake::LEFT && dir_ != Snake::RIGHT))
			return false;
		dir_ = Snake::RIGHT;
	}

	else if (event->buttonPressed(BUTTON_LEFT)) {
		if (!(dir_ != Snake::RIGHT && dir_ != Snake::LEFT && dir_ != Snake::START))
			return false;
		dir_ = Snake::LEFT;
	}

	else if (event->buttonPressed(BUTTON_DOWN)) {
		if (!(dir_ != Snake::UP && dir_ != Snake::DOWN))
			return false;
		dir_ = Snake::DOWN;
	} else
		return false;

	// avoid quick jumping of the snake.
	// if the time passed since the last move forward is too smal
	// accept the position but do not move again
	if (timer_->triggered() || (timer_->nextTime() - (timer_->interval() / 2)) > millis()) {
		return false;
	}
	timer_->restart();
	return tick();
}

bool Snake::onTriggered(Event* event) {
	if (timer_->triggered()) {
		return tick();
	}
	return false;
}

void Snake::onStop(Event* event) {
	// stop timer during break
	timer_->stop();
	// let the stop symbol and score print
	Game::onStop(event);
}

void Snake::onContinue(Event* event) {
	// restart the stopped timer
	timer_->start();
	// executes render and clears the score text
	Game::onContinue(event);
}

void Snake::render() {
	// first of all, clear the display
	display_->clear();
	// set the eat pixel
	display_->setPixel(eat_pos_);
	// interate through all body pixels
	for (SmartPos tmp : body_) {
		// now set those pixels
		display_->setPixel(tmp.toPos());
	}
	// finaly, show on display
	display_->show(false);
}

bool Snake::eat(const Pos pos) {
	// check if eat was received
	if (pos != eat_pos_)
		return false;

	// prevent entering a remaining loop when the game is won
	if (body_.size() >= display_->rows() * display_->cols())
		return true;
	unsigned long time = millis();

	// temporary new pos
	Pos tmp;
	do {
		wdt_reset();
		// generate a random pos
		tmp = Pos((time + char(rand())) % 8, (time + (char(rand()))) % 16);
		// try as long as the position doesnt match
	} while (!isValid(tmp) || tmp == pos);

	// when free position is found, set it as the new eat
	eat_pos_ = tmp;
	return true;
}

bool Snake::tick() {
	// figure out move vector
	Pos vect;
	switch (dir_) {
		case UP:
			vect = Pos(0, 1);
			break;
		case DOWN:
			vect = Pos(0, -1);
			break;
		case RIGHT:
			vect = Pos(1, 0);
			break;
		case LEFT:
			vect = Pos(-1, 0);
			break;
		default:
			return false;
			break;
	}

	// new head
	// already remove it, to ensure a true validation
	vect += Pos(body_.last());

	// move head inside the game field
	validate(vect);

	// process eat
	if (!eat(vect)) {
		// only remove the Pixel, when we didnt eat, ohterwise the snake will grow
		body_.removeFirst();
	}
	// check for new highscore. Directly save to avoid a not save in case of
	// reset or poweroff.
	if (score() + 1 > highscore_)  // + 1 because we didnt add the next vect because of the isValid() function
	{
		// write new highscore
		highscore_ = score() + 1;
		// save new highscore in eeprom
		eeprom_write_word(&EE_highscore_, highscore_);
		is_new_highscore_ = true;
	}

	// check for colidation
	if (!isValid(vect)) {
		// append the buffer to ensure correct score(computed out of the size of the list)
		body_ << vect.toSmartPos();
		return true;
	}
	body_ << vect.toSmartPos();

	render();
	return false;
}

// check if the given position is valid,
// so isnt touching any point of the snake
bool Snake::isValid(const Pos& pos) const {
	//  if colides, return true
	for (SmartPos tmp : body_) {
		if (pos == tmp.toPos()) {
			return false;
		}
	}
	return true;
}

// moves the given direction into the specified game field
// and check if the position is valid afterwards
bool Snake::validate(Pos& pos) {
	// out of range
	if (pos.pos_x < 0)
		pos.pos_x = display_->cols() - 1;
	else if (pos.pos_x > display_->cols() - 1)
		pos.pos_x = 0;
	if (pos.pos_y < 0)
		pos.pos_y = display_->rows() - 1;
	else if (pos.pos_y > display_->rows() - 1)
		pos.pos_y = 0;
	return isValid(pos);
}
