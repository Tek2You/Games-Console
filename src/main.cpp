/* main.cpp : main file executing all needed systems and containing interruptroutines
 *
 * Copyright (C) 2019 Felix Haschke
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.

 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see http://www.gnu.org/licenses/.
 */

#include <avr.h>
#include <avr/wdt.h>
#include <display.h>
#include <event.h>
#include <menu_sm.h>

/* needed Fuses: 
 *	LFUSE_DEFAULT | (byte)~FUSE_CKDIV8,  // run at 8MHz
 *	HFUSE_DEFAULT& FUSE_EESAVE,  // protect EEPROM from erase
 *	EFUSE_DEFAULT,
 */

Event event;
Display dp(16, 8);
MenuSM sm(&dp, &event);

bool check_buttons = false;
void initHardware();

int main(void)
{
	initHardware();
	wdt_disable();
	wdt_enable(WDTO_500MS);

	while (1)
	{
		wdt_reset();

		if (check_buttons)
		{
			event.checkButtons();
			check_buttons = false;
		}
		event.processDebounce();

		if (event.process())
		{
			sm.process(&event);
		}
		event.clear();
		dp.update();
	}
	return 0;
}

// pin change interrupt routine for buttons
ISR(PCINT1_vect)
{
	// set flag for later button check
	check_buttons = true;
}

ISR(PCINT0_vect)
{
	// set flag for later button check
	check_buttons = true;
}

// display-show timer

ISR(TIMER2_COMPA_vect)
{
	dp.processShow();
	TCNT2 = 255;
}

void initHardware()
{
	// setup button registers
	DDRC &= ~0b0000;
	PORTC |= 0b1111;
	DDRB |= 0b10;
	PORTB |= 0b10;

	// enable pin change interrupts for buttons
	PCMSK1 |= 0x0F;
	PCMSK0 |= (1 << PCINT0);
	PCICR |= (1 << PCIE1) | (1 << PCIE0);

	// init avr.h interface
	init();

	// setup timer for display showing
	TCNT2 = 255;
	TCCR2A |= (1 << WGM21);
	TIMSK2 |= (1 << OCIE2A);
	TCCR2B |= (1 << CS20);
	// enable global interrupts
	sei();
}