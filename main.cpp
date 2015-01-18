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

#include "lib/telemetry/uavtalk/uavtalk.h"

inline void init ()
{
	settings::init ();
	timer::init ();
	uart0::init (UART_BAUD_SELECT (UART_BAUD_RATE));
	spi::init ();
	max7456::init ();
	adc::init ();
	telemetry::init ();
	osd::init ();
}

int main ()
{
	init ();

//	max7456::open (0, 0);
//	fprintf_P (&max7456::stream, PSTR ("TEST"));
//	max7456::clear ();

	uint32_t _last_display = 0;
	while (true)
	{
		uint32_t ticks = timer::ticks ();
		bool updated = telemetry::update ();
		if (updated && (_last_display + 50 <= ticks))
		{
			max7456::wait_vsync ();

			max7456::clear ();

			_last_display = ticks;

			max7456::open (1, 1);
			fprintf_P (&max7456::stream, PSTR ("Updated at %04x%04x"), (uint16_t) (ticks >> 16), (uint16_t) ticks);

			max7456::open (1, 3);
			fprintf_P (&max7456::stream,
				PSTR ("CS:%d A:%c"),
				telemetry::status::connection,
				telemetry::status::armed ? 'A' : 'D'
			);
			max7456::open (1, 7);
			fprintf_P (&max7456::stream, PSTR ("\x87:%03d \xb2:%03d \xb1:%03d Y:%03d"),
				telemetry::input::throttle,
				telemetry::input::roll,
				telemetry::input::pitch,
				telemetry::input::yaw
			);
			max7456::open (1, 8);
			fprintf_P (&max7456::stream, PSTR ("r:%3.2f p:%3.2f y:%3.2f  "),
				telemetry::attitude::roll,
				telemetry::attitude::pitch,
				telemetry::attitude::yaw
			);
			max7456::open (1, 9);
			fprintf_P (&max7456::stream, PSTR ("V:%03.2f A:%03.2f C:%u"),
				telemetry::battery::voltage,
				telemetry::battery::current,
				telemetry::battery::consumed
			);
		}
	}

	return 0;
}

