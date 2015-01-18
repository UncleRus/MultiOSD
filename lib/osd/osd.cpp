#include "osd.h"
#include <avr/pgmspace.h>
#include "../max7456/max7456.h"
#include <stdio.h>
#include "../../config.h"
#include "../uart/uart.h"
#include "../timer/timer.h"
#include <util/delay.h>

namespace osd
{

#define _OSD_LOGO_WIDTH 5
#define _OSD_LOGO_HEIGHT 2

#define OSD_CONFIG_WAIT_TIME 5000

void get_cmd (char *buf, uint8_t len)
{
	uint8_t i = 0;
	while (true)
	{
		uint16_t data = uart0::receive ();
		if (data & 0xff00) continue;
		if (data == '\n' || i == len - 1) data = 0;
		buf [i ++] = data;
		if (!data) return;
	}
}

uint8_t init ()
{
	// boot sequence
	max7456::clear ();

	max7456::open_hcenter (18, 1);
	fprintf_P (&max7456::stream, PSTR ("MIN_raw_OSD v.%04u"), VERSION);

	max7456::open_center (_OSD_LOGO_WIDTH, _OSD_LOGO_HEIGHT);
	fprintf_P (&max7456::stream, PSTR ("\xBA\xBB\xBC\xBD\xBE\n\xCA\xCB\xCC\xCD\xCE"));

	max7456::open_hcenter (22, max7456::bottom - 1);
	fprintf_P (&max7456::stream, PSTR ("send \"CONFIG\" or wait %us"), OSD_CONFIG_WAIT_TIME / 1000);

	max7456::open (1, 3);
#ifdef TELEMETRY_MODULES_UAVTALK
	fprintf_P (&max7456::stream, PSTR ("\x08""UAVtalk\n"));
#endif
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	fprintf_P (&max7456::stream, PSTR ("\xb9""ADCbatt\n"));
#endif
#ifdef TELEMETRY_MODULES_I2C_BARO
	fprintf_P (&max7456::stream, PSTR ("\xb1""I2Cbaro\n"));
#endif

	uart0::send_string_p (PSTR ("READY\r\n"));

	char data [10];
	get_cmd (data, 10);
	max7456::open (1, max7456::bottom - 2);
	fprintf (&max7456::stream, data);

	_delay_ms (5000);
	return 0;

	/*
	// 16bit should be enough
	char cmd [8];
	char *pos = &cmd [0];
	uint16_t start = timer::ticks ();
	while (timer::ticks () <= start + OSD_CONFIG_WAIT_TIME)
	{
		uint16_t data = uart0::receive ();
		if (data & 0xff00) continue;
		*(pos ++) = data;
		if (data == '\n')
			return OSD_MODE_CONFIG;
		//if ((char) data == 'C') return OSD_MODE_CONFIG;
	}
	return OSD_MODE_FLIGHT;
	*/
}


}
