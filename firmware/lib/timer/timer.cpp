/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

namespace timer
{

volatile uint32_t _ticks = 0;

void init ()
{
	// timer0 OC02 off, CTC mode
	TCCR0A = _BV (WGM01);
	// timer0 prescaler = 64
	TCCR0B = _BV (CS00) | _BV (CS01);
	// timer0 counter = 0
	TCNT0 = 0;
	// timer0 TOP = (Fosc / prescaler) * 0.001s
	OCR0A = F_CPU / 64 / 1000;
	// compare interrupt
	TIMSK0 |= _BV (OCIE0A);
	sei ();
}

SIGNAL (TIMER0_COMPA_vect)
{
	_ticks ++;
}

uint32_t ticks ()
{
	cli ();
	uint32_t res = _ticks;
	sei ();
	return res;
}

}
