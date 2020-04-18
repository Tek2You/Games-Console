/* event.h : Library provides timers and buttons for statemaschine
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
#include <buttonevent.h>
#include <flags/flags.h>
#include <genericevent.h>

enum Button
{
	BUTTON_LEFT = _BV(0),
	BUTTON_DOWN = _BV(1),
	BUTTON_UP = _BV(2),
	BUTTON_RIGHT = _BV(3),
	BUTTON_STOP = _BV(4),
	CONTROL_BUTTON = BUTTON_LEFT | BUTTON_DOWN | BUTTON_UP | BUTTON_RIGHT,
	ANY_BUTTON = CONTROL_BUTTON | BUTTON_STOP
};

enum CustomTriggerFlag
{
	GameMode = (event_handling::ProcessPinChanges | event_handling::ProcessTriggers),
	MenuMode = event_handling::ProcessPinChanges,
};

class Game;

class Event : public ButtonEvent
{
public:
	friend class Game;

	Event();
	~Event();

	// button methods
	void processDebounce();
	void checkButtons();

	//	uint32_t buttonChanges() const { return changes_; }
protected:
	byte debounce_timers_[5];
};
