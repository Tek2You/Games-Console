/* game_sm.h : Statemaschine class containing functions for every state
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
#include <statemachine.h>
#include <event.h>
#include <game.h>

class Display;
class MenuSM : public StateMachine<Event*>
{
public:
	MenuSM(Display* display, Event* event);
	//	void processStateMaschine(Event *event);
private:
	typedef void (MenuSM::*Function)(Event*);
	void transition(Function function, Event* event, events::EntryMode entry = events::ForwardEntry);

	bool processMenuStop(Event* event);
	// states
	void stateStartup(Event* event);
	void stateDefault(Event* event);
	void stateGameOver(Event* event);
	void stateLoadEffect(Event* event);
	void stateSettingsMenu(Event* event);
	void stateSpeedMenu(Event* event);
	void stateBrightnessMenu(Event* event);
	void stateLanguageMenu(Event* event);
	void stateHighscoreMenu(Event* event);
	void stateResetMenu(Event* event);

	void newGame(Event* event, Game::GameType game);
	void stateGame(Event* event);

	struct Item
	{
		const char* text_[2];
		const Display::Icon icon_;
	};

	void showItem(const Item& item);

	class MenuItem
	{
	public:
		void init(char num, char initial, events::EntryMode entry = events::ForwardEntry) {
			num_ = num;
			if (entry == events::ForwardEntry)
				value_ = initial;
		}
		void init(char num) { num_ = num; }

		enum Button
		{
			NO_BTN = 0,
			UP_BTN = 1,
			DOWN_BTN = 2,
		};

		bool advance(Event* event, bool overflow = true);
		bool advance(bool diretion, bool overflow = true);
		char value_ = 0;
		char num_ = 0;
	};

	enum Language
	{
		EN = 0,
		DE = 1,
	};

	Display* display_;
	// pointer towards current game
	Game* game_;
	Language language_ = EN;
	byte speed_ = 0;
	byte brightness_ = 0;

	// load effect
	State load_following_state_;  // state load effect need a pointer to the state
	                              // after its
	events::EntryMode load_following_entry_mode_;
};
