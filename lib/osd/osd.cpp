#include "osd.h"
#include <avr/pgmspace.h>
#include "../max7456/max7456.h"
#include <stdio.h>
#include "../../config.h"
#include "../uart/uart.h"
#include "../timer/timer.h"

namespace osd
{

#define _OSD_LOGO_WIDTH 5
#define _OSD_LOGO_HEIGHT 2

#define OSD_CONFIG_WAIT_TIME 5000

uint8_t init ()
{
	// boot sequence
	max7456::clear ();

	max7456::open_hcenter (18, 1);
	fprintf_P (&max7456::stream, PSTR ("MIN_raw_OSD v.%04u"), VERSION);

	max7456::open_center (_OSD_LOGO_WIDTH, _OSD_LOGO_HEIGHT);
	fprintf_P (&max7456::stream, PSTR ("\xBA\xBB\xBC\xBD\xBE\n\xCA\xCB\xCC\xCD\xCE"));

	max7456::open_hcenter (19, max7456::bottom - 1);
	fprintf_P (&max7456::stream, PSTR ("send \"C\" or wait %us"), OSD_CONFIG_WAIT_TIME / 1000);

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

	// 16bit should be enough
//	uint16_t start = timer::ticks ();
//	while (timer::ticks () <= start + OSD_CONFIG_WAIT_TIME)
//	{
//		uint16_t data = uart0::receive ();
//		if (data & 0xff00) continue;
//		if ((char) data == 'C') return OSD_MODE_CONFIG;
//	}
	return OSD_MODE_FLIGHT;
}


}
