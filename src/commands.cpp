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
#include "commands.h"
#include <avr/pgmspace.h>
#include <util/delay.h>
#include <stdlib.h>
#include "config.h"
#include "lib/console/console.h"
#include "lib/uart/uart.h"
#include "lib/max7456/max7456.h"
#include "settings.h"
#include "osd/panel.h"
#include "telemetry/telemetry.h"

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
	const char *arg = console::argument (1);
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

void exec ()
{
	CONSOLE_UART::send_string_p (PSTR ("Reset to defaults... "));
	settings::reset ();
	CONSOLE_UART::send_string_p (_str_done);
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

void _set ()
{
	const char *saddr = console::argument (3);
	const char *value = console::argument (4);
	if (saddr && value)
	{
		uint16_t addr = atoi (saddr);
		if (addr >= EEPROM_SIZE)
		{
			fprintf_P (&CONSOLE_UART::stream, PSTR ("Max addr %u"), EEPROM_SIZE);
			return;
		}
		switch (*argument (2))
		{
			case 'b':
			case 'B':
				eeprom_update_byte ((uint8_t *) addr, atoi (value));
				return;
			case 'i':
			case 'I':
				eeprom_update_word ((uint16_t *) addr, atoi (value));
				return;
			case 'w':
			case 'W':
				eeprom_update_word ((uint16_t *) addr, atol (value));
				return;
			case 'l':
			case 'L':
				eeprom_update_dword ((uint32_t *) addr, atol (value));
				return;
			case 'f':
			case 'F':
				eeprom_update_float ((float *) addr, atof (value));
				return;
		}
	}
	CONSOLE_UART::send_string_p (PSTR ("Args: <b|i|w|l|f> <addr> <value>"));
}

void _get ()
{
	const char *saddr = console::argument (3);
	if (saddr)
	{
		uint16_t addr = atoi (saddr);
		if (addr >= EEPROM_SIZE)
		{
			fprintf_P (&CONSOLE_UART::stream, PSTR ("Max addr %u"), EEPROM_SIZE);
			return;
		}
		switch (*argument (2))
		{
			case 'b':
			case 'B':
				fprintf_P (&CONSOLE_UART::stream, PSTR ("%u"), eeprom_read_byte ((uint8_t *) addr));
				return;
			case 'i':
			case 'I':
				fprintf_P (&CONSOLE_UART::stream, PSTR ("%i"), eeprom_read_word ((uint16_t *) addr));
				return;
			case 'w':
			case 'W':
				fprintf_P (&CONSOLE_UART::stream, PSTR ("%u"), eeprom_read_word ((uint16_t *) addr));
				return;
			case 'l':
			case 'L':
				char buf [13];
				ltoa (eeprom_read_dword ((uint32_t *) addr), buf, 10);
				CONSOLE_UART::send_string (buf);
				return;
			case 'f':
			case 'F':
				fprintf_P (&CONSOLE_UART::stream, PSTR ("%f"), eeprom_read_float ((float *) addr));
				return;
		}
	}
	CONSOLE_UART::send_string_p (PSTR ("Args: <b|i|w|l|f> <addr>"));
}

void exec ()
{
	const char *arg = console::argument (1);
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
			case 's':
			case 'S':
				_set ();
				return;
			case 'g':
			case 'G':
				_get ();
				return;
		}
	}
	CONSOLE_UART::send_string_p (PSTR ("Args: d - dump, r - read, w - write"));
}

}  // namespace eeprom

namespace info
{

const char __cmd [] PROGMEM = "info";

void exec ()
{
	fprintf_P (&CONSOLE_UART::stream, PSTR ("VERSION: %04u\r\n"), VERSION);
	CONSOLE_UART::send_string_p (PSTR ("MODULES: "));
	telemetry::fprintf_build (&CONSOLE_UART::stream, ' ');
	console::eol ();
	CONSOLE_UART::send_string_p (PSTR ("PANELS:\r\n"));
	for (uint8_t i = 0; i < OSD_PANELS_COUNT; i ++)
	{
		fprintf_P (&CONSOLE_UART::stream, PSTR ("%03u: "), i);
		CONSOLE_UART::send_string_p (osd::panel::name_p (i));
		console::eol ();
	}
}

}  // namespace info

#define _cmd_is_P(x) (!strncasecmp_P (command, x, size))

void process (const char *cmd)
{
	const char *command = console::argument (0);
	uint8_t size = 0;
	while (command [size] && command [size] != ' ') size ++;

	if      (_cmd_is_P (font::__cmd))   font::exec ();
	else if (_cmd_is_P (reset::__cmd))  reset::exec ();
	else if (_cmd_is_P (eeprom::__cmd)) eeprom::exec ();
	else if (_cmd_is_P (info::__cmd))   info::exec ();
	else if (_cmd_is_P (PSTR ("exit"))) stop ();

	else CONSOLE_UART::send_string_p (PSTR ("Invalid command"));
}

}
