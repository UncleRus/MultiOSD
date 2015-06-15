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
#include "../firmware/settings.h"

#include "../firmware/config.h"
#include "../firmware/lib/adc/adc.h"
#include "../firmware/lib/max7456/max7456.h"
#include "../firmware/osd/osd.h"
#include "../firmware/telemetry/telemetry.h"

namespace settings
{

void init ()
{
	if (eeprom_read_word (NULL) != EEPROM_HEADER || eeprom_read_word ((uint16_t *) 2) != VERSION)
		reset ();
}

void reset ()
{
	adc::settings::reset ();
	max7456::settings::reset ();
	telemetry::settings::reset ();
	osd::settings::reset ();

	eeprom_write_word (NULL, EEPROM_HEADER);
	eeprom_write_word ((uint16_t *) 2, VERSION);
}

}
