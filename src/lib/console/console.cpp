#include "console.h"
#include <stddef.h>
#include <stdlib.h>
#include <avr/pgmspace.h>
#include "../uart/uart.h"

#define KEY_ENTER	0x0D
#define KEY_BS		0x08
#define KEY_DEL		0x7F
#define KEY_ESC		0x1B

#include "../../config.h"

#ifndef CONSOLE_UART
#	define CONSOLE_UART uart0
#endif

#ifndef CONSOLE_MAX_CMD_LENGTH
#	define CONSOLE_MAX_CMD_LENGTH (UART_RX_BUFFER_SIZE - 1)
#endif

namespace console
{


static uint8_t _command_len = 0;
callback_t handler = NULL;
static char _command [CONSOLE_MAX_CMD_LENGTH];

void eol ()
{
	CONSOLE_UART::send_string_p (PSTR (CONSOLE_EOL));
}

void show_prompt ()
{
	eol ();
	CONSOLE_UART::send_string_p (PSTR (CONSOLE_PROMPT));
	_command_len = 0;
}

const char *str_argument (uint8_t position, const char *def)
{
	uint8_t offs = 0;
	while (_command [offs] && _command [offs] == ' ') offs ++;
	for (uint8_t i = 0; i < position; i ++)
	{
		while (_command [offs] && _command [offs] != ' ') offs ++;
		while (_command [offs] && _command [offs] == ' ') offs ++;
	}
	return _command [offs] ? &_command [offs] : def;
}

int32_t int_argument (uint8_t position)
{
	return atol (str_argument (position));
}

bool _process_byte ()
{
	uint16_t data = CONSOLE_UART::receive ();
	if (data & 0xff00) return false;
	uint8_t byte = data & 0xff;
	switch (byte)
	{
		case KEY_ENTER:
			PORTB = 0x00;
			if (_command_len)
			{
				_command [_command_len] = 0;
				eol ();
				if (handler) handler (_command);
			}
			show_prompt ();
			return true;
		case KEY_DEL:
		case KEY_BS:
			if (!_command_len) return true;
			_command_len --;
			CONSOLE_UART::send_string_p (PSTR ("\x08 \x08"));
			return true;
		default:
			if (byte < 0x20 || _command_len == CONSOLE_MAX_CMD_LENGTH - 1)
			{
				CONSOLE_UART::send (0x07);
				return true;
			}
			_command [_command_len ++] = byte;
			CONSOLE_UART::send (byte);
	}
	return true;
}

bool running;

void run (callback_t h)
{
	show_prompt ();
	running = true;
	handler = h;
	while (running) _process_byte ();
}

}
