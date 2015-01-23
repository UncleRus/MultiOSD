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
#include "config.h"
#include "settings.h"

#include "lib/timer/timer.h"
#include "lib/uart/uart.h"
#include "lib/spi/spi.h"
#include "lib/max7456/max7456.h"
#include "lib/adc/adc.h"
#include "lib/mtwi/mtwi.h"
#include "telemetry/telemetry.h"
#include "lib/console/console.h"
#include "commands.h"
#include "boot.h"
#include "osd/osd.h"

inline void init ()
{
	settings::init ();
	timer::init ();
	uart0::init (UART_BAUD_SELECT (UART_BAUD_RATE));
	spi::init ();
	max7456::init ();
	adc::init ();
	if (boot::show ())
	{
		max7456::clear ();
		max7456::puts_p (max7456::hcenter - 4, max7456::vcenter, PSTR ("\xfc CONFIG"));
		console::run (console::process);
	}
	uart0::send_string_p (PSTR ("BOOT\n"));
	telemetry::init ();
	osd::init ();
}

int main ()
{
	init ();

	osd::main ();

	return 0;
}

