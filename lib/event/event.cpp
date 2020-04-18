/* event.cpp : Library provides timers and buttons for statemaschine
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

#include <event.h>

using namespace event_handling;

Event::Event() : ButtonEvent(), debounce_timers_{ 0, 0, 0, 0, 0 } {}

Event::~Event() {}

bool readButton(Button b) {
	return !(b == BUTTON_STOP ? bitRead(PINB, 1) : b & PINC);
}

void Event::processDebounce() {
	for (int i = 0; i <= 4; i++) {
		if (debounce_timers_[i] > 0) {
			debounce_timers_[i]--;
			if (debounce_timers_[i] == 0) {
				Button button = Button(_BV(i));
				bool read = readButton(button);
				if (read != states_.testFlag(button)) {
					states_.setFlag(button, read);
				}
				changes_.setFlag(button);
			}
		}
	}
}

void Event::checkButtons() {
	for (int i = 0; i <= 4; i++) {
		Button b = Button(_BV(i));
		if (bool(debounce_timers_[i]) != (readButton(b) != states_.testFlag(b)))
			debounce_timers_[i] = 20;
	}
}