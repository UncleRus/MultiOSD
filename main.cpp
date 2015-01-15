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

#include "lib/timer/timer.h"
#include "lib/uart/uart.h"
#include "lib/spi/spi.h"
#include "lib/max7456/max7456.h"
#include "lib/adc/adc.h"
#include "lib/mtwi/mtwi.h"
#include "lib/settings/settings.h"
#include "lib/osd/osd.h"
#include "lib/telemetry/telemetry.h"

inline void init ()
{
	settings::init ();
	uart0::init (UART_BAUD_SELECT (19200));
	timer::init ();
	spi::init ();
	max7456::init ();
	adc::init ();

	telemetry::init ();
}

int main ()
{
	init ();

	osd::init ();

	while (true)
	{
		//mtwi::exec_p (PSTR (""));
		//_delay_ms (500);
		//if (telemetry::update ())
		//{
//			max7456::open (0, 1);
//			fprintf_P (&max7456::stream, PSTR ("UPDATED at %u"), timer::ticks ());
//			max7456::close ();
			fprintf_P (&uart0::stream, PSTR ("time %u\r\n"), timer::ticks ());
		//}
		_delay_ms (500);
	}

	return 0;
}

