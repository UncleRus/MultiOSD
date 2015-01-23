#include "boot.h"
#include <avr/pgmspace.h>
#include "config.h"
#include "lib/timer/timer.h"
#include "lib/uart/uart.h"
#include "lib/max7456/max7456.h"

#define _BOOT_LOGO_WIDTH 5
#define _BOOT_LOGO_HEIGHT 2

#define _BOOT_CMD_BUF_SIZE 8

namespace boot
{

void _get_cmd (char *buf, uint8_t len, uint16_t timeout)
{
	uint8_t i = 0;
	uint32_t stop = timer::ticks () + timeout;
	while (true)
	{
		uint16_t data = timer::ticks () < stop ? CONSOLE_UART::receive () : 0;
		if (data & 0xff00) continue;
		if (data == 0x0d || i == len - 1) data = 0;
		buf [i ++] = data;
		if (!data) break;
	}
}

bool show ()
{
	// boot sequence
	max7456::clear ();

	max7456::open_hcenter (18, 1);
	fprintf_P (&max7456::stream, PSTR ("MIN_RAW_OSD v.%04u"), VERSION);

	max7456::open_center (_BOOT_LOGO_WIDTH, _BOOT_LOGO_HEIGHT);
	fprintf_P (&max7456::stream, PSTR ("\xBA\xBB\xBC\xBD\xBE\n\xCA\xCB\xCC\xCD\xCE"));

	max7456::open (1, max7456::bottom - 1);
	fprintf_P (&max7456::stream, PSTR ("send \""BOOT_CONFIG_CODE"\" or wait %us"), BOOT_CONFIG_WAIT_TIME / 1000);

	max7456::open (1, 3);
#ifdef TELEMETRY_MODULES_UAVTALK
	fprintf_P (&max7456::stream, PSTR ("\xfc""UAVtalk\n"));
#endif
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	fprintf_P (&max7456::stream, PSTR ("\xf7""ADCbatt\n"));
#endif
#ifdef TELEMETRY_MODULES_I2C_BARO
	fprintf_P (&max7456::stream, PSTR ("\x85""I2Cbaro\n"));
#endif

	CONSOLE_UART::send_string_p (PSTR ("READY\r\n"));

	char data [_BOOT_CMD_BUF_SIZE];
	_get_cmd (data, _BOOT_CMD_BUF_SIZE, BOOT_CONFIG_WAIT_TIME);
	return !strncmp_P (data, PSTR (BOOT_CONFIG_CODE), _BOOT_CMD_BUF_SIZE);
}

}
