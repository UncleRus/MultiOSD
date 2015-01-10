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
#include "mtwi.h"
#include <avr/io.h>
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "../timer/timer.h"
#include "../../config.h"

namespace mtwi
{

#define send_start()		{ TWCR = _BV (TWINT) | _BV (TWSTA) | _BV (TWEN); }
#define is_start_sent		((TWSR & 0xf8) == 0x08 || (TWSR & 0xf8) == 0x10)
#define send_stop()			{ TWCR = _BV (TWINT) | _BV (TWSTO) | _BV (TWEN); }
#define send_byte(b) 		{ TWDR = b; TWCR = _BV (TWINT) | _BV (TWEN); }

#define ask_byte()			{ TWCR = _BV (TWINT) | _BV (TWEN) | _BV (TWEA); }
#define ask_last_byte()		{ TWCR = _BV (TWINT) | _BV (TWEN); }
#define is_byte_received	((TWSR & 0xf8) == 0x50 || (TWSR & 0xf8) == 0x58)
#define is_byte_acked		((TWSR & 0xf8) == 0x18 || (TWSR & 0xf8) == 0x40 || (TWSR & 0xf8) == 0x28 || (TWSR & 0xf8) == 0x50)


static uint8_t _rx_position = 0;
static uint8_t _rx_buffer [MTWI_RX_BUFFER_SIZE];
volatile static uint8_t error = 0;

uint8_t utils::hex2nibble (char hex)
{
	hex -= '0';
	if (hex < 0) return 0;
	if (hex < 10) return hex;
	hex -= 7;
	if (hex < 10) return 0;
	if (hex < 16) return hex;
	hex -= 32;
	return hex > 9 && hex < 16 ? hex : 0;
}

void _wait ()
{
	uint32_t start = timer::ticks ();
	bool timeout = false;
	while (!(TWCR & _BV (TWINT)) && !timeout)
		timeout = timer::ticks () >= start + MTWI_TIMEOUT;
	if (timeout) error = MTWI_ERR_TIMEOUT;
}

bool exists (uint8_t addr)
{
	error = 0;
	send_start ();
	_wait ();
	if (!is_start_sent)
		error = MTWI_ERR_BUS_FAIL;
	if (error) return false;
	send_byte (addr);
	_wait ();
	if (error) return false;
	bool result = is_byte_acked;
	send_stop ();
	return result;
}

uint8_t _exec (char cmd, char arg)
{
	if (!cmd) return 0;
	switch (cmd)
	{
		case 's':
			// start, repeated start
			send_start ();
			_wait ();
			if (error) return 0;
			if (!is_start_sent)
			{
				error = MTWI_ERR_BUS_FAIL;
				return 0;
			}
			return 1;
		case 'p':
			// stop
			send_stop ();
			return 1;
		case ' ':
			// nothing to do
			return 1;
		case '.':
			// delay 10 ms
			_delay_ms (10);
			return 1;
		case '-':
			// delay 100 ms
			_delay_ms (100);
			return 1;
		case 'S':
			// set speed
			switch (arg)
			{
				case 'H':
					// high speed - 400 kHz
					TWBR = 0x02;
					break;
				case 'L':
					// low speed - 15 kHz
					TWBR = 0xff;
					break;
				default:
					// default speed - 100 kHz
					TWBR = 0x20;
					break;
			}
			return 2;
		case '?':
			// ask byte
			if (_rx_position == MTWI_RX_BUFFER_SIZE)
			{
				error = MTWI_ERR_BUFFER_OVERFLOW;
				return 0;
			}
			if (arg == '?') ask_byte ()
				else ask_last_byte ();
			_wait ();
			if (error) return 0;
			if (!is_byte_received)
			{
				error = MTWI_ERR_BUSY;
				return 0;
			}
			_rx_buffer [_rx_position ++] = TWDR;
			return 1;
	}
	// send byte by default
	if (!arg)
	{
		error = MTWI_ERR_INVALID_COMMAND;
		return 0;
	}
	send_byte ((utils::hex2nibble (cmd) << 4) | utils::hex2nibble (arg));
	_wait ();
	if (error) return 0;
	if (!is_byte_acked)
	{
		error = MTWI_ERR_NOT_ACKED;
		return 0;
	}
	return 2;
}

const uint8_t *exec (const char *cmd)
{
	error = 0;
	_rx_position = 0;

	while (*cmd && !error)
		cmd += _exec (*cmd, *(cmd + 1));

	return _rx_buffer;
}

const uint8_t *exec_p (const char *progmem_cmd)
{
	error = 0;
	_rx_position = 0;
	char cmd;

	while ( (cmd = pgm_read_byte (progmem_cmd)) && !error)
		progmem_cmd += _exec (cmd, pgm_read_byte (progmem_cmd + 1));

	return _rx_buffer;
}

}

