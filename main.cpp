#include <avr/io.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "config.h"

#include "lib/timer/timer.h"
#include "lib/uart/uart.h"
#include "lib/spi/spi.h"
#include "lib/max7456/max7456.h"
#include "lib/adc/adc.h"

#include "lib/telemetry/telemetry.h"


inline void init ()
{
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

	max7456::open (0, 0, MAX7456_ATTR_BLINK);
	fprintf_P (&max7456::stream, PSTR ("Hello world %.2f, %d"), 3.141592, timer::ticks ());
	max7456::close ();

	uart0::send_string_p (PSTR ("Hello!\r\n"));

	while (true)
	{
		if (telemetry::update ())
		{
			max7456::open (0, 1);
			fprintf_P (&max7456::stream, PSTR ("UPDATED at %u"), timer::ticks ());
			max7456::close ();
		}
		_delay_ms (10);
	}

	return 0;
}

