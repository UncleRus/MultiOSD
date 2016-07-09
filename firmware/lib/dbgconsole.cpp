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
#include "dbgconsole.h"
#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>

#define DEBUG_CONSOLE_CNT (F_CPU / DEBUG_CONSOLE_BAUDRATE / DEBUG_CONSOLE_PRESCALER)
#define DEBUG_CONSOLE_TX_BUF_MASK (DEBUG_CONSOLE_TX_BUF_SIZE - 1)

#if (DEBUG_CONSOLE_CNT > 0xff || DEBUG_CONSOLE_CNT == 0)
	#error TCNT overflow
#endif

#if ((F_CPU * 100 / DEBUG_CONSOLE_CNT / DEBUG_CONSOLE_PRESCALER / DEBUG_CONSOLE_BAUDRATE) != 100)
	#error Baud rate error > 1%
#endif

#if (DEBUG_CONSOLE_PRESCALER == 1)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS20))
#elif (DEBUG_CONSOLE_PRESCALER == 8)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS21))
#elif (DEBUG_CONSOLE_PRESCALER == 32)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS21) | _BV (CS20))
#elif (DEBUG_CONSOLE_PRESCALER == 64)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS22))
#elif (DEBUG_CONSOLE_PRESCALER == 128)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS22) | _BV (CS20))
#elif (DEBUG_CONSOLE_PRESCALER == 256)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS22) | _BV (CS21))
#elif (DEBUG_CONSOLE_PRESCALER == 1024)
	#define DEBUG_CONSOLE_PRESCALER_VAL (_BV (CS22) | _BV (CS21) | _BV (CS20))
#else
	#error Invalid prescaler value
#endif

namespace dbgconsole
{

FILE stream;

volatile uint8_t _tx_buf [DEBUG_CONSOLE_TX_BUF_SIZE];
volatile uint8_t _tx_head;
volatile uint8_t _tx_tail;
volatile uint8_t _tx_bits;
volatile uint16_t _tx_value;

int _fputc (char c, FILE *s)
{
	send (c);
	return 0;
}

int _fgetc (FILE *s)
{
	return 0;
}

void init ()
{
	_tx_head = 0;
	_tx_tail = 0;
	_tx_bits = 0;

	DEBUG_CONSOLE_TX_DDR |= _BV (DEBUG_CONSOLE_TX_BIT);
	DEBUG_CONSOLE_TX_PORT |= _BV (DEBUG_CONSOLE_TX_BIT);

	TCNT2 = 0;
	OCR2A = DEBUG_CONSOLE_CNT;
	TCCR2A = _BV (WGM21);
	TCCR2B = DEBUG_CONSOLE_PRESCALER_VAL;
	TIMSK2 = _BV (OCIE2A);
	sei ();

	fdev_setup_stream (&stream, _fputc, _fgetc, _FDEV_SETUP_RW);
}

ISR (TIMER2_COMPA_vect)
{
	if (_tx_head == _tx_tail) return;

	if (!_tx_bits)
	{
		// get next byte
		_tx_value = (((uint16_t) _tx_buf [_tx_tail]) << 1) | 0x200;
		// 10 bits to send
		_tx_bits = 10;
	}

	// sending
	if (_tx_value & 1) DEBUG_CONSOLE_TX_PORT |= _BV (DEBUG_CONSOLE_TX_BIT);
		else DEBUG_CONSOLE_TX_PORT &= ~_BV (DEBUG_CONSOLE_TX_BIT);

	_tx_value >>= 1;
	_tx_bits --;

	// byte sent, moving tail
	if (!_tx_bits)
	{
		_tx_tail = (_tx_tail + 1) & DEBUG_CONSOLE_TX_BUF_MASK;
		DEBUG_CONSOLE_TX_PORT |= _BV (DEBUG_CONSOLE_TX_BIT);
	}
}

void send (uint8_t b)
{
	// new head pos
	uint8_t head = (_tx_head + 1) & DEBUG_CONSOLE_TX_BUF_MASK;

	// waiting for free space in buffer
	while (head == _tx_tail)
		;

	_tx_head = head;
	_tx_buf [head] = b;
}

void send_string (const char *s)
{
	while (*s)
		send (*s ++);
}

void send_string_p (const char *progmem_s)
{
	register char c;

	while ((c = pgm_read_byte (progmem_s ++)))
		send (c);
}

}  // namespace dbgconsole
