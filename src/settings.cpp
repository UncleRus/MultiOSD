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
#include "settings.h"
#include "lib/max7456/max7456.h"
#include "telemetry/telemetry.h"

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
