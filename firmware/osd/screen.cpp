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

#include "screen.h"
#include <avr/eeprom.h>
#include <avr/pgmspace.h>
#include "../config.h"
#include "../lib/max7456/max7456.h"
#include "panel.h"
#include "../eeprom.h"

namespace osd
{

namespace screen
{

panel_pos_t records [OSD_SCREEN_PANELS];
uint8_t count = 0;

uint8_t *eeprom_offset (uint8_t num, uint8_t panel)
{
	return _eeprom_byte (OSD_SCREENS_EEPROM_OFFSET + num * sizeof (panel_pos_t) * OSD_SCREEN_PANELS  + panel * sizeof (panel_pos_t));
}

void load (uint8_t num)
{
	if (num >= OSD_MAX_SCREENS) num = 0;

	uint8_t *offset = eeprom_offset (num);

	uint8_t i = 0;
	for (; i < OSD_SCREEN_PANELS; i ++)
	{
		records [i].panel = eeprom_read_byte (offset);
		if (records [i].panel >= osd::panel::count)
			break;
		records [i].x = eeprom_read_byte (offset + 1);
		records [i].y = eeprom_read_byte (offset + 2);
		offset += sizeof (panel_pos_t);
	}
	count = i;

	max7456::clear ();
}

volatile bool updated = false;

void update ()
{
	for (uint8_t i = 0; i < count; i ++)
		osd::panel::update (records [i].panel);
	updated = true;
}

void draw ()
{
	if (!updated) return;
	max7456::clear ();
	for (uint8_t i = 0; i < count; i ++)
		osd::panel::draw (records [i].panel, records [i].x, records [i].y);
	updated = false;
}

namespace settings
{

const panel_pos_t _default_screen_0 [] PROGMEM = {
	{OSD_PANEL_GPS_STATE, 1, 1},
	{OSD_PANEL_GPS_LAT, 8, 1},
	{OSD_PANEL_GPS_LON, 19, 1},

	{OSD_PANEL_CONNECTION_STATE, 0, 2},
	{OSD_PANEL_ARMING_STATE, 3, 2},
	{OSD_PANEL_RSSI, 21, 3},
	{OSD_PANEL_FLIGHT_MODE, 24, 2},

	{OSD_PANEL_PITCH, 1, 6},
	{OSD_PANEL_HORIZON, 8, 6},
	{OSD_PANEL_THROTTLE, 23, 6},

	{OSD_PANEL_GROUND_SPEED, 1, 8},
	{OSD_PANEL_ALT, 23, 8},

	{OSD_PANEL_ROLL, 1, 10},
	{OSD_PANEL_CLIMB, 23, 10},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},
	{OSD_PANEL_COMPASS, 9, 12},

	{OSD_PANEL_HOME_DISTANCE, 1, 14},
	{OSD_PANEL_HOME_DIRECTION, 8, 14},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 12},
	{OSD_PANEL_BATTERY_CURRENT, 22, 13},
	{OSD_PANEL_BATTERY_CONSUMED, 22, 14}
};

#if OSD_MAX_SCREENS > 1
const panel_pos_t _default_screen_1 [] PROGMEM = {
	{OSD_PANEL_CONNECTION_STATE, 0, 0},
	{OSD_PANEL_ARMING_STATE, 3, 0},
	{OSD_PANEL_CALLSIGN, 7, 1},
	{OSD_PANEL_RSSI, 21, 1},
	{OSD_PANEL_FLIGHT_MODE, 24, 0},

	{OSD_PANEL_ALT, 1, 8},
	{OSD_PANEL_HORIZON, 8, 6},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},
	{OSD_PANEL_HOME_DISTANCE, 1, 14},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 14}
};
#endif

#if OSD_MAX_SCREENS > 2
const panel_pos_t _default_screen_2 [] PROGMEM = {
	{OSD_PANEL_CONNECTION_STATE, 0, 0},
	{OSD_PANEL_ARMING_STATE, 3, 0},
	{OSD_PANEL_RSSI, 20, 1},
	{OSD_PANEL_FLIGHT_MODE, 24, 0},

	{OSD_PANEL_PITCH, 1, 6},
	{OSD_PANEL_ALT, 1, 8},
	{OSD_PANEL_ROLL, 1, 10},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 14}
};
#endif


void reset_screen (uint8_t num, const panel_pos_t screen [], uint8_t len)
{
	uint8_t *offset = eeprom_offset (num);
	for (uint8_t i = 0; i < OSD_SCREEN_PANELS; i ++, offset += sizeof (panel_pos_t))
	{
		if (i < len)
		{
			eeprom_update_byte (offset, pgm_read_byte (&(screen [i].panel)));
			eeprom_update_byte (offset + 1, pgm_read_byte (&(screen [i].x)));
			eeprom_update_byte (offset + 2, pgm_read_byte (&(screen [i].y)));
		}
		else
		{
			eeprom_update_byte (offset, 0xff);
			eeprom_update_byte (offset + 1, 0xff);
			eeprom_update_byte (offset + 2, 0xff);
		}
	}
}

void reset ()
{
	reset_screen (0, _default_screen_0, sizeof (_default_screen_0) / sizeof (panel_pos_t));
#if OSD_MAX_SCREENS > 1
	reset_screen (1, _default_screen_1, sizeof (_default_screen_1) / sizeof (panel_pos_t));
#endif
#if OSD_MAX_SCREENS > 2
	reset_screen (2, _default_screen_2, sizeof (_default_screen_2) / sizeof (panel_pos_t));
#endif
}

}  // namespace settings

}  // namespace screen

}  // namespace osd
