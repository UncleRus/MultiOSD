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
#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "lib/timer/timer.h"

#include "../firmware/boot.h"
#include "../firmware/commands.h"
#include "../firmware/config.h"
#include "../firmware/lib/adc/adc.h"
#include "../firmware/lib/console/console.h"
#include "../firmware/lib/max7456/max7456.h"
#include "../firmware/lib/spi/spi.h"
#include "../firmware/lib/timer/timer.h"
#include "../firmware/lib/uart/uart.h"
#include "../firmware/osd/osd.h"
#include "../firmware/settings.h"
#include "../firmware/telemetry/telemetry.h"

inline void init ()
{
	settings::init ();
	timer::init ();
	CONSOLE_UART::init (UART_BAUD_SELECT (UART_BAUD_RATE));
	spi::init ();
	max7456::init ();
	if (boot::show ())
	{
		max7456::clear ();
		max7456::puts_p (max7456::hcenter - 4, max7456::vcenter, PSTR ("\xfc CONFIG"));
		fprintf_P (&CONSOLE_UART::stream, PSTR ("MultiOSD v.%04u\r\n"), VERSION);
		console::run (console::process);
	}
	CONSOLE_UART::send_string_p (PSTR ("BOOT\r\n"));
	adc::init ();
	telemetry::init ();
	osd::init ();
}

int main ()
{
	init ();

	osd::main ();

	return 0;
}

