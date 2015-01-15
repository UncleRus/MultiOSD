#include "osd.h"
#include <avr/pgmspace.h>
#include "../max7456/max7456.h"
#include <stdio.h>
#include "../../config.h"

namespace osd
{

#define _OSD_LOGO_WIDTH 5
#define _OSD_LOGO_HEIGHT 2

uint8_t init ()
{
	// boot sequence
	max7456::clear ();

	max7456::open_hcenter (18, 1);
	fprintf_P (&max7456::stream, PSTR ("MIN_raw_OSD v.%04u"), VERSION);

	max7456::open_center (_OSD_LOGO_WIDTH, _OSD_LOGO_HEIGHT);
	fprintf_P (&max7456::stream, PSTR ("\xBA\xBB\xBC\xBD\xBE|\xCA\xCB\xCC\xCD\xCE"));

	max7456::open_hcenter (24, max7456::bottom - 1);
	fprintf_P (&max7456::stream, PSTR ("send \"CONFIG\" or wait 5s"));

	//uart0::send_string_p (PSTR (">>READY\n"));

	return OSD_MODE_FLIGHT;
}


}
