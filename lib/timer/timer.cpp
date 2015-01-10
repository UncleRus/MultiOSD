#include "timer.h"
#include <avr/io.h>
#include <avr/interrupt.h>

namespace timer
{

volatile static uint32_t _ticks = 0;

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
