#include "settings.h"
#include "../max7456/max7456.h"
#include "../telemetry/telemetry.h"

namespace settings
{

void init ()
{
	if (eeprom_read_word (NULL) != EEPROM_HEADER)
		reset ();
}

void reset ()
{
	max7456::settings::reset ();
	telemetry::settings::reset ();

	eeprom_write_word (NULL, EEPROM_HEADER);
	// FIXME: remove after debugging
	//eeprom_write_word (NULL, 0);
}

}
