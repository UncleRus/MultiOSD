/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * MultiOSD is free software: you can redistribute it and/or modify
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
#include "console.h"
#include <stddef.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../lib/uart/uart.h"

#define KEY_ENTER	0x0D
#define KEY_BS		0x08
#define KEY_DEL		0x7F
#define KEY_ESC		0x1B

#include "../config.h"

#ifndef CONSOLE_UART
#	define CONSOLE_UART uart0
#endif

#ifndef CONSOLE_MAX_CMD_LENGTH
#	define CONSOLE_MAX_CMD_LENGTH (UART_RX_BUFFER_SIZE - 1)
#endif

namespace console
{

uint8_t _command_len = 0;
callback_t handler = NULL;
char _command [CONSOLE_MAX_CMD_LENGTH];

void eol ()
{
	CONSOLE_UART::send_string_p (PSTR (CONSOLE_EOL));
}

void show_prompt ()
{
	CONSOLE_UART::send_string_p (PSTR (CONSOLE_EOL CONSOLE_PROMPT));
	_command_len = 0;
}

const char *argument (uint8_t position, const char *def)
{
	uint8_t offs = 0;
	while (_command [offs] && _command [offs] == ' ') offs++;
	for (uint8_t i = 0; i < position; i++)
	{
		while (_command [offs] && _command [offs] != ' ') offs++;
		while (_command [offs] && _command [offs] == ' ') offs++;
	}
	return _command [offs] ? &_command [offs] : def;
}

void read_argument (const char *start, char *dest)
{
	uint8_t c;
	while ((c = *start))
	{
		if (c != ' ') *(dest++) = c;
		else break;
		start++;
	}
	*(dest++) = 0;
}

void _process_byte ()
{
	uint16_t data = CONSOLE_UART::receive ();
	if (data & 0xff00) return;
	uint8_t byte = data & 0xff;
	switch (byte)
	{
		case KEY_ENTER:
			if (_command_len)
			{
				_command [_command_len] = 0;
				eol ();
				if (handler) handler (_command);
			}
			show_prompt ();
			return;
		case KEY_DEL:
		case KEY_BS:
			if (!_command_len) return;
			_command_len--;
			CONSOLE_UART::send_string_p (PSTR ("\x08 \x08"));
			return;
		default:
			if (byte < 0x20 || _command_len == CONSOLE_MAX_CMD_LENGTH - 1)
			{
				CONSOLE_UART::send (0x07);
				return;
			}
			_command [_command_len++] = byte;
			CONSOLE_UART::send (byte);
	}
	return;
}

bool running;

void run (callback_t h)
{
	show_prompt ();
	running = true;
	handler = h;
	while (running)
		_process_byte ();
}

}
