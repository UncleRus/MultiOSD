#include "commands.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#include "config.h"
#include "lib/console/console.h"
#include "lib/uart/uart.h"
#include "lib/max7456/max7456.h"
#include "settings.h"

namespace console
{

const char _str_done [] PROGMEM = "Done.\r\n";

uint8_t _read ()
{
	while (true)
	{
		uint16_t c = CONSOLE_UART::receive ();
		if (!(c & 0xff00)) return c;
	}
}

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
		CONSOLE_UART::send ((b >> i) & 1 ? '1' : '0');
	eol ();
}

void _download ()
{
	_draw ();
	CONSOLE_UART::send_string_p (PSTR ("MAX7456\r\n"));
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

uint8_t _read_byte ()
{
	uint8_t res = 0;
	for (uint8_t i = 0; i < 8; i ++)
		res = (res << 1) | ((console::_read () - '0') & 1);
	// \r\n
	console::_read ();
	console::_read ();
	return res;
}

void _upload ()
{
	_draw ();
	CONSOLE_UART::send_string_p (PSTR ("Send MCM-file\r\n"));
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
	CONSOLE_UART::send_string_p (_str_done);
}

void exec ()
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
	CONSOLE_UART::send_string_p (PSTR ("Args: u - upload, d - download"));
}

} // namespace font

namespace reset
{

const char __cmd [] PROGMEM = "reset";

const char _please_reboot [] PROGMEM = "PLEASE REBOOT";

void exec ()
{
	CONSOLE_UART::send_string_p (PSTR ("Reset to defaults... "));
	settings::reset ();
	CONSOLE_UART::send_string_p (_str_done);

	max7456::clear ();
	max7456::puts_p (max7456::hcenter - 6, max7456::vcenter, _please_reboot, MAX7456_ATTR_BLINK);

	while (true)
	{
		CONSOLE_UART::send_string_p (_please_reboot);
		console::eol ();
		_delay_ms (1000);
	}
}

}  // namespace reset

namespace eeprom
{

const char __cmd [] PROGMEM = "eeprom";

void _dump ()
{
	for (uint8_t row = 0; row < EEPROM_SIZE / 16; row ++)
	{
		for (uint8_t byte = 0; byte < 16; byte ++)
			fprintf_P (&CONSOLE_UART::stream, PSTR ("%02x "), eeprom_read_byte ((uint8_t *) ((row << 4) | byte)));
		console::eol ();
	}
}

void _read ()
{
	for (uint16_t addr = 0; addr < EEPROM_SIZE; addr ++)
		CONSOLE_UART::send (eeprom_read_byte ((uint8_t *) addr));
}

void _write ()
{
	for (uint16_t addr = 0; addr < EEPROM_SIZE; addr ++)
		eeprom_update_byte ((uint8_t *) addr, console::_read ());
}

void exec ()
{
	const char *arg = str_argument (1);
	if (arg)
	{
		switch (*arg)
		{
			case 'd':
			case 'D':
				_dump ();
				return;
			case 'r':
			case 'R':
				_read ();
				return;
			case 'w':
			case 'W':
				_write ();
				return;
		}
	}
	CONSOLE_UART::send_string_p (PSTR ("Args: d - dump, r - read, w - write"));
}

}  // namespace eeprom

#define _cmd_is_P(x) (!strncasecmp_P (command, x, size))

void process (const char *cmd)
{
	const char *command = console::str_argument (0);
	uint8_t size = 0;
	while (command [size] && command [size] != ' ') size ++;

	if      (_cmd_is_P (font::__cmd))   font::exec ();
	else if (_cmd_is_P (reset::__cmd))  reset::exec ();
	else if (_cmd_is_P (eeprom::__cmd)) eeprom::exec ();
	else if (_cmd_is_P (PSTR ("exit"))) stop ();

	else CONSOLE_UART::send_string_p (PSTR ("Invalid command"));
}

}
