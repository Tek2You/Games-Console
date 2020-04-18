/* tetris.h : Tetris game library
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

#pragma once
#include <avr.h>
#include <buttonautotrigger.h>
#include <display.h>
#include <game.h>
#include <staticlist.h>
#include <tetromino.h>

class Tetris : public Game
{
public:
	explicit Tetris(Display* display);
	~Tetris();

	void start(Event* event) override;
	void setSpeed(const byte v) override { speed_ = v; }

	// score functions
	unsigned int score() const override { return points_; }

	static unsigned int highscore() { return highscore_; }
	static void resetHighscore();

protected:
	// game interface
	bool onButtonChange(Event* event) override;
	bool onTriggered(Event* event) override;
	void onStop(Event* event) override;
	void onContinue(Event* event) override;
	void render() override;

private:
	// private functions only for tetris
	void rotate();
	void move(const ButtonAutoTrigger::Direction dir);
	bool tick(Event* event);

	bool clearFullRows(Event* event);
	bool rowsFull() const;

	bool newTetromino();
	void takeOverTetromino();

	// field without current tetromino
	StaticList<byte> field_;
	Tetromino tetromino_;

	Timer* step_timer_;
	Timer* blink_timer_;
	ButtonAutoTrigger* move_trigger_;

	enum SpeedFlag
	{
		StepInterval = 0,
		DownInterval = 1,
		FirstMoveInterval = 2,
		MoveInterval = 3,
		BlinkInterval = 4
	};

	byte speed_;
	uint16_t readSpeed(const SpeedFlag flag) const;

	uint16_t points_ = 0;
	static uint16_t EE_highscore_ EEMEM;
	static uint16_t highscore_;

	enum BlinkCycle
	{
		DEFAULT = 0,
		INIT_BLINK = 1,
		BLINK_OFF_1 = 2,
		BLINK_ON_1 = 3,
		BLINK_OFF_2 = 4,
		BLINK_ON_2 = 5,
		BLINK_OFF_3 = 6,
		FINISHED_CURRENT = 7
	};
	BlinkCycle blink_cycle_;
	byte blink_start_row_;
	byte blink_end_row_;
};
