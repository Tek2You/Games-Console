﻿/* game_sm.cpp : game statemaschine class containing functions for every state
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
#include <defaults.h>
#include <dodge.h>
#include <menu_sm.h>
#include <operators.h>
#include <snake.h>
#include <spaceinvaders.h>
#include <tetris.h>

using namespace events;

#undef TRANSITION
#define TRANSITION(s, event, entry) transition(&MenuSM::s, event, entry)

#define LOAD_EFFECT_STANDART(s, event, entry)         \
	{                                                  \
		load_following_entry_mode_ = entry;             \
		load_following_state_ = STATE_CAST(&MenuSM::s); \
		TRANSITION(stateLoadEffect, event, entry);      \
		return;                                         \
	}

#define LOAD_EFFECT_BEGIN(s, event)                    \
	{                                                   \
		static bool load_passed = false;                 \
		if (load_passed) {                               \
			load_passed = false;                          \
		} else {                                         \
			load_passed = true;                           \
			LOAD_EFFECT_STANDART(s, event, ForwardEntry); \
		}                                                \
	}

static byte EE_speed EEMEM = DEFAULT_SPEED;
static byte EE_language EEMEM = DEFAULT_LANGUAGE;
static byte EE_brightness EEMEM = DEFAULT_BRIGHTNESS;

MenuSM::MenuSM(Display* display, Event* event)
  : StateMachine(STATE_CAST(&MenuSM::stateStartup))
  , display_(display)
  , game_(nullptr)
  , load_following_state_(nullptr)
  , load_following_entry_mode_(ForwardEntry) {
	display_->text1_.setShiftSpeed(5);
	display_->text2_.setShiftSpeed(5);
	display_->text1_.setAlignment(Text::MIDDLE);
	display_->text2_.setAlignment(Text::MIDDLE);
	speed_ = eeprom_read_byte(&EE_speed);
	language_ = (eeprom_read_byte(&EE_language) ? DE : EN);
	brightness_ = (eeprom_read_byte(&EE_brightness));
	if (brightness_ > 3) {
		brightness_ = 3;
	}
	display->setBrightness(brightness_);
	event->setOnEntry();
	process(event);
	event->clear();
}

void MenuSM::transition(Function function, Event* event, events::EntryMode entry) {
	setState(static_cast<StateMachine<Event*>::State>(function));
	event->setOnEntry(entry);
	event->triggerModes().clear();
	this->StateMachine::process(event);
}

bool MenuSM::processMenuStop(Event* event) {
	if (event->buttonPressed(BUTTON_STOP)) {
		TRANSITION(stateDefault, event, ForwardEntry);
		return true;
	}
	return false;
}

bool MenuSM::MenuItem::advance(Event* event, bool overflow) {
	if (event->buttonPressed(BUTTON_RIGHT)) {
		if (++value_ >= num_)
			value_ = (overflow ? 0 : num_ - 1);
		return true;
	}
	if (event->buttonPressed(BUTTON_LEFT)) {
		if (--value_ < 0)
			value_ = (overflow ? num_ - 1 : 0);
		return true;
	}
	return false;
}

bool MenuSM::MenuItem::advance(bool diretion, bool overflow) {
	if (diretion) {
		if (++value_ >= num_)
			value_ = (overflow ? 0 : num_ - 1);
		return true;
	} else {
		if (--value_ < 0)
			value_ = (overflow ? num_ - 1 : 0);
		return true;
	}
	return false;
}

void MenuSM::stateStartup(Event* event) {
	static int count;
	if (event->onEntry()) {
		count = 0;
		event->triggerModes().setFlag(GameMode);
		event->addTrigger(new Timer(100));
	} else if (event->buttonChanged(ANY_BUTTON)) {
		TRANSITION(stateDefault, event, ForwardEntry);
		return;
	} else if (event->trigger(0)->triggered()) {
		switch (count) {
			case 0:
				display_->setPixel(Pos(5, 13));
				break;
			case 1:
				display_->setPixel(Pos(4, 13));
				break;
			case 2:
				display_->setPixel(Pos(3, 13));
				break;
			case 3:
				display_->setPixel(Pos(3, 12));
				break;
			case 4:
				display_->setPixel(Pos(2, 12));
				break;
			case 5:
				display_->setPixel(Pos(2, 11));
				break;
			case 6:
				display_->setPixel(Pos(1, 11));
				display_->setPixel(Pos(2, 10));
				break;
			case 7:
				display_->setPixel(Pos(1, 10));
				break;
			case 8:
				display_->setPixel(Pos(5, 11));
				break;
			case 9:
				display_->setPixel(Pos(6, 10));
				break;
			case 10:
				display_->setPixel(Pos(5, 10));
				break;
			case 11:
				display_->setPixel(Pos(5, 9));
				break;
			case 12:
				display_->setPixel(Pos(5, 8));
				break;
			case 13:
				display_->setPixel(Pos(4, 8));
				break;
			case 14:
				display_->setPixel(Pos(4, 7));
				break;
			case 15:
				display_->setPixel(Pos(3, 7));
				display_->setPixel(Pos(4, 6));
				break;
			case 16:
				display_->setPixel(Pos(3, 6));
				break;
			case 17:
				display_->setPixel(Pos(3, 5));
				break;
			case 18:
				display_->setPixel(Pos(2, 5));
				break;
			case 19:
				display_->setPixel(Pos(2, 4));
				break;
			case 20:
				display_->setPixel(Pos(1, 4));
				display_->setPixel(Pos(2, 3));
				break;
			case 21:
				display_->setPixel(Pos(1, 3));
				break;
			case 22:
				display_->setPixel(Pos(2, 3));
				break;
			case 23:
				display_->setPixel(Pos(7, 5));
				break;
			case 24:
				display_->setPixel(Pos(6, 5));
				break;
			case 25:
				display_->setPixel(Pos(6, 4));
				break;
			case 26:
				display_->setPixel(Pos(5, 4));
				break;
			case 27:
				static_cast<Timer*>(event->trigger(0))->setInterval(2000);
				static_cast<Timer*>(event->trigger(0))->restart();
				break;
			case 28:
				TRANSITION(stateDefault, event, ForwardEntry);
				return;
			default:
				break;
		}
		count++;
		display_->show();
	}
}

void MenuSM::stateDefault(Event* event) {
	static const Item items[6] = { Item{ "highscore", "Highscore", Display::Icon(0x00081c2018043810) },
		                            Item{ "settings", "Einstellungen", Display::Icon(0x000003c3c3c3c0000) },
		                            Item{ "Tetris", "Tetris", Display::Icon(0xfffff7e300081c00) },
		                            Item{ "Snake", "Snake", Display::Icon(0x3c20203c04045c00) },
		                            Item{ "Space Invaders", "Space Invaders", Display::Icon(0x1c08000841d46b91) },
		                            Item{ "Dodge", "Dodge", Display::Icon(0x381003c00e30310) } };

	static MenuItem item;

	if (event->onEntry()) {
		item.init(6, 2, event->onEntry());
		display_->loadMenuConfig();
		event->triggerModes().setFlag(MenuMode);
	} else {
		if (event->buttonPressed(BUTTON_DOWN)) {
			switch (item.value_) {
				// last because we begin counting with 2
				case 0:
					TRANSITION(stateHighscoreMenu, event, ForwardEntry);
					break;
				case 1:
					TRANSITION(stateSettingsMenu, event, ForwardEntry);
					break;
				default:
					newGame(event, Game::GameType(item.value_ - 2));
					TRANSITION(stateGame, event, ForwardEntry);
					break;
			}
			return;
		}
		if (event->buttonPressed(BUTTON_STOP)) {
			item.value_ = 2;
		} else if (!item.advance(event))
			return;
	}

	showItem(items[(byte)item.value_]);
}

void MenuSM::newGame(Event* event, Game::GameType game) {
	switch (game) {
		case Game::TETRIS:
			game_ = new Tetris(display_);
			break;
		case Game::SNAKE:
			game_ = new Snake(display_);
			break;
		case Game::SPACE_INVADERS:
			game_ = new SpaceInvaders(display_);
			break;
		case Game::DODGE:
			game_ = new Dodge(display_);
			break;
		default:
			return;
	}
}

void MenuSM::stateGame(Event* event) {
	if (event->onEntry()) {
		display_->loadsGameCofig();
		event->triggerModes().setFlag(GameMode);
		game_->setSpeed(speed_);
		game_->start(event);
		return;
	} else if (game_->process(event)) {
		TRANSITION(stateGameOver, event, ForwardEntry);
	}
}

void MenuSM::stateGameOver(Event* event) {
	if (event->onEntry()) {
		LOAD_EFFECT_BEGIN(stateGameOver, event);
		display_->loadsGameCofig();
		if (game_ != nullptr) {
			event->triggerModes().setFlag(MenuMode);
			display_->loadMenuConfig();
			if (game_->type() == Game::SNAKE && game_->score() >= 124) {
				display_->text1_.setText((language_ == EN ? "you got it!" : "Geschafft!"), false);
			} else {
				if (game_->isNewHighscore()) {
					display_->text1_.setText((language_ == EN ? "new highscore!" : "Neuer Highscore!"), false);
				} else {
					display_->text1_.setText("Game Over", false);
				}
			}
			display_->text2_.setNumber(game_->score(), false);
			display_->show();
			// delete game
			delete game_;
			game_ = nullptr;
		}
		return;
	} else  // not on entry
	{
		if (event->buttonPressed(ANY_BUTTON)) {
			TRANSITION(stateDefault, event, BackwardEntry);
			return;
		}
		processMenuStop(event);
	}
}

void MenuSM::stateSettingsMenu(Event* event) {
	static const Item items[3] = { Item{ "speed", "Geschwindigkeit", Display::Icon(0x0000122448241200) },
		                            Item{ "brightness", "Helligkeit", Display::Icon(0x0007133558900000) },
		                            Item{ "language", "Sprache", Display::Icon(0x2060ff818181ff00) } };

	static MenuItem item;

	if (event->onEntry()) {
		display_->loadMenuConfig();
		item.init(3, 0, event->onEntry());
		event->triggerModes().setFlag(MenuMode);
	} else  // not on entry
	{
		if (processMenuStop(event)) {
			return;
		}

		if (event->buttonPressed(BUTTON_DOWN)) {
			event->setOnEntry();
			switch (item.value_) {
				case 0:
					TRANSITION(stateSpeedMenu, event, ForwardEntry);
					break;
				case 1:
					TRANSITION(stateBrightnessMenu, event, ForwardEntry);
					break;
				case 2:
					TRANSITION(stateLanguageMenu, event, ForwardEntry);
					break;
				default:
					break;
			}
			return;
		}

		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateDefault, event, BackwardEntry);
			return;
		}

		if (!item.advance(event))
			return;
	}

	showItem(items[(byte)item.value_]);
}

void MenuSM::stateSpeedMenu(Event* event) {
	static MenuItem item;

	if (event->onEntry()) {
		item.init(5, speed_);
		display_->loadMenuConfig();
		event->triggerModes().setFlag(GameMode);
		event->addTrigger(new ButtonAutoTrigger(event, BUTTON_LEFT, BUTTON_RIGHT, 500, 300));
	} else  // not on entry
	{
		if (processMenuStop(event)) {
			return;
		}
		if (event->buttonPressed(BUTTON_DOWN)) {
			speed_ = item.value_;
			eeprom_write_byte(&EE_speed, speed_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		ButtonAutoTrigger* auto_trigger = static_cast<ButtonAutoTrigger*>(event->trigger(0));
		if (auto_trigger->triggered()) {
			item.advance(auto_trigger->direction() == ButtonAutoTrigger::BTN_2, false);
		}
	}

	display_->setBar(item.value_ + 1, item.num_);
	display_->text1_.setNumber((byte)(item.value_) + 1);
}

void MenuSM::stateBrightnessMenu(Event* event) {
	static MenuItem item;

	if (event->onEntry()) {
		display_->loadMenuConfig();
		item.init(4, brightness_);
		event->triggerModes().setFlag(GameMode);
		event->addTrigger(new ButtonAutoTrigger(event, BUTTON_LEFT, BUTTON_RIGHT, 500, 400));
	} else  // not on entry
	{
		if (processMenuStop(event)) {
			return;
		}
		if (event->buttonPressed(BUTTON_DOWN)) {
			brightness_ = item.value_;
			display_->setBrightness(brightness_);
			eeprom_write_byte(&EE_brightness, brightness_);
			LOAD_EFFECT_STANDART(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		ButtonAutoTrigger* auto_trigger = static_cast<ButtonAutoTrigger*>(event->trigger(0));
		if (auto_trigger->triggered()) {
			item.advance(auto_trigger->direction() == ButtonAutoTrigger::BTN_2, false);
		}
	}

	display_->setBar(item.value_ + 1, item.num_);
	display_->text1_.setNumber(item.value_ + 1);
	display_->setBrightness(item.value_);
}

void MenuSM::stateLanguageMenu(Event* event) {
	static MenuItem item;

	if (event->onEntry()) {
		item.init(2, (language_ == DE ? 0 : 1));
		display_->loadMenuConfig();
		event->triggerModes().setFlag(MenuMode);
	} else  // not on entry
	{
		if (processMenuStop(event)) {
			return;
		}
		if (event->buttonPressed(BUTTON_DOWN)) {
			language_ = Language(item.value_);
			eeprom_write_byte(&EE_language, byte(language_));
			LOAD_EFFECT_STANDART(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateSettingsMenu, event, BackwardEntry);
			return;
		}
		if (!item.advance(event))
			return;
	}

	display_->text1_.setText(item.value_ == 0 ? "english" : "Deutsch", false);
	display_->text2_.setText(item.value_ == 0 ? "E" : "D");
}

void MenuSM::stateLoadEffect(Event* event) {
	static byte count = 0;
	if (event->onEntry()) {
		count = 0;
		display_->loadsGameCofig();
		event->addTrigger(new Timer(12));
		event->triggerModes().setFlag(ProcessTriggers);
		event->triggerModes().setFlag(ProcessStop);
	} else {
		if (processMenuStop(event)) {
			return;
		}
		if (event->trigger(0)->triggered()) {
			if (count >= display_->rows() * display_->cols() / 2) {
				if (load_following_state_) {
					setState(load_following_state_);
					load_following_state_ = nullptr;
					event->triggerModes().clear();
					event->setOnEntry(load_following_entry_mode_);
					process(event);
				} else {
					TRANSITION(stateDefault, event, ForwardEntry);
				}
				return;
			}
			display_->setPixel(display_->cols() - 1 - (count % display_->cols()), count / display_->cols(), true);
			display_->setPixel(count % display_->cols(), display_->rows() - 1 - (count / display_->cols()), true);
			display_->show();
			count++;
			return;
		}
	}
}

void MenuSM::stateHighscoreMenu(Event* event) {
	static MenuItem item;

	if (event->onEntry()) {
		item.init(5, 1, event->onEntry());
		event->triggerModes().setFlag(MenuMode);
	} else {
		if (processMenuStop(event)) {
			return;
		}
		if (event->buttonPressed(BUTTON_DOWN)) {
			if (item.value_ == 0) {
				TRANSITION(stateResetMenu, event, ForwardEntry);
			} else {
				TRANSITION(stateDefault, event, BackwardEntry);
			}
			return;
		}
		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateDefault, event, BackwardEntry);
			return;
		}
		if (!item.advance(event)) {
			return;
		}
	}
	Display::Icon icon(0);
	int highscore;

	switch (item.value_) {
		case 0:
			display_->setIcon(Display::Icon(0xbd42a59999a542bd), 0, false);
			display_->text1_.setText(language_ == EN ? "reset" :
			                                           "Zur"
			                                           "\x1c"
			                                           "cksetzen");
			return;
		case 1:
			icon = Display::Icon(0xfffff7e300081c00);
			highscore = Tetris::highscore();
			break;
		case 2:
			icon = Display::Icon(0x3c20203c04045c00);
			highscore = Snake::highscore();
			break;
		case 3:
			icon = Display::Icon(0x1c08000841d46b91);
			highscore = SpaceInvaders::highscore();
			break;
		case 4:
			icon = Display::Icon(0x381003c00e30310);
			highscore = Dodge::highscore();
			break;
		default:
			break;
			;
	}
	display_->setIcon(icon, false);
	display_->text1_.setNumber(highscore);
}

void MenuSM::stateResetMenu(Event* event) {
	if (event->onEntry()) {
		display_->loadMenuConfig();
		event->triggerModes().setFlag(MenuMode);
	} else {
		if (processMenuStop(event)) {
			return;
		}
		if (event->buttonPressed(BUTTON_DOWN)) {
			Tetris::resetHighscore();
			Snake::resetHighscore();
			Dodge::resetHighscore();
			SpaceInvaders::resetHighscore();
			LOAD_EFFECT_STANDART(stateDefault, event, BackwardEntry);
			return;
		}
		if (event->buttonPressed(BUTTON_UP)) {
			TRANSITION(stateHighscoreMenu, event, BackwardEntry);
			return;
		}
	}
	display_->clear();
	display_->text1_.setText((language_ == EN ? "reset scores" :
	                                            "Highscores zur"
	                                            "\x1c"
	                                            "cksetzen"),
	                         false);
	display_->setIcon(Display::Icon(Display::Icon(0x00040a1120408000)), false);
	display_->show();
}

void MenuSM::showItem(const MenuSM::Item& item) {
	display_->setIcon(item.icon_, 0, false);

	display_->text1_.setText(item.text_[language_]);
	display_->show();
}
