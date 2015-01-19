#include "commands.h"
#include <avr/pgmspace.h>
#include "lib/console/console.h"
#include "lib/uart/uart.h"
#include "lib/max7456/max7456.h"

namespace console
{


namespace font
{

const char __cmd [] PROGMEM = "font";

void _draw ()
{
	max7456::clear ();
	for (uint8_t h = 0; h < 0x10; h ++)
		for (uint8_t l = 0; l < 0x10; l ++)
			max7456::put (max7456::hcenter - 8 + l, h, (h << 4) | l);
}

void _print_byte (uint8_t b)
{
	for (uint8_t i = 0; i < 8; i ++)
		uart0::send ((b >> i) & 1 ? '1' : '0');
	eol ();
}

void _download ()
{
	_draw ();
	uart0::send_string_p (PSTR ("MAX7456\r\n"));
	for (uint16_t c = 0; c < 0x100; c ++)
	{
		uint8_t data [54];
		max7456::download_char (c, data);
		for (uint8_t i = 0; i < 54; i ++)
			_print_byte (data [i]);
		for (uint8_t i = 0; i < 10; i ++)
			_print_byte (0);
	}
}

uint8_t _read ()
{
	while (true)
	{
		uint16_t c = uart0::receive ();
		if (!(c & 0xff00)) return c;
	}
}

uint8_t _read_byte ()
{
	uint8_t res = 0;
	for (uint8_t i = 0; i < 8; i ++)
		res = (res << 1) | ((_read () - '0') & 1);
	// \r\n
	_read ();
	_read ();
	return res;
}

void _upload ()
{
	_draw ();
	uart0::send_string_p (PSTR ("Send MCM-file\r\n"));
	for (uint8_t i = 0; i < 9; i ++)
		_read ();
	for (uint16_t c = 0; c < 0x100; c ++)
	{
		uint8_t data [54];
		for (uint8_t i = 0; i < 54; i ++)
			data [i] = _read_byte ();
		for (uint8_t i = 0; i < 10; i ++)
			_read_byte ();
		max7456::upload_char (c, data);
	}
	uart0::send_string_p (PSTR ("Done\r\n"));
}

void exec (const char *cmd)
{
	const char *arg = str_argument (1);
	if (arg)
	{
		switch (*arg) {
			case 'u':
			case 'U':
				_upload ();
				return;
			case 'd':
			case 'D':
				_download ();
		}		return;
	}
	uart0::send_string_p (PSTR ("Args: u - upload, d - download"));
}

}

#define _cmd_is_P(x) (!strncasecmp_P (command, x, size))

void process (const char *cmd)
{
	const char *command = console::str_argument (0);
	uint8_t size = 0;
	while (command [size] && command [size] != ' ') size ++;

	if (_cmd_is_P (font::__cmd)) font::exec (cmd);
	else if (_cmd_is_P (PSTR ("exit"))) stop ();

	else uart0::send_string_p (PSTR ("Invalid command"));
}

}
