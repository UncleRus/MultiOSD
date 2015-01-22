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
#include "settings.h"
#include "telemetry/telemetry.h"
#include "lib/console/console.h"
#include "commands.h"
#include "boot.h"

#include "telemetry/uavtalk/uavtalk.h"
#include "osd/panel.h"

inline void init ()
{
	settings::init ();
	timer::init ();
	uart0::init (UART_BAUD_SELECT (UART_BAUD_RATE));
	spi::init ();
	max7456::init ();
	adc::init ();
	if (boot::show ()) console::run (console::process);
	uart0::send_string_p (PSTR ("BOOT\n"));
	telemetry::init ();
	max7456::clear ();
}

int main ()
{
	init ();

	uint32_t _last_display = 0;
	while (true)
	{
		uint32_t ticks = timer::ticks ();
		bool updated = telemetry::update ();
		if (updated && (_last_display + 34 <= ticks))
		{
			max7456::wait_vsync ();
			max7456::clear ();

			_last_display = ticks;

			osd::draw_panel (6, 1, 1);
			osd::draw_panel (7, 8, 1);
			osd::draw_panel (1, 16, 1);
			osd::draw_panel (0, 24, 1);

			osd::draw_panel (2, 0, 2);
			osd::draw_panel (3, 8, 2);
			osd::draw_panel (4, 11, 2);

			osd::draw_panel (5, 1, 5);

			osd::draw_panel (8, 1, 6);
			osd::draw_panel (9, 8, 6);
			osd::draw_panel (10, 19, 6);

			//max7456::wait_vsync ();
			osd::draw_panel (11, 8, 7);

		}
	}

	return 0;
}

