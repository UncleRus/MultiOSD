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
#include "../settings.h"
#include "../config.h"
#include "../lib/max7456/max7456.h"
#include "panel.h"

namespace osd
{

namespace screen
{

struct _panel_pos_t
{
	uint8_t panel;
	uint8_t x;
	uint8_t y;
};

static _panel_pos_t _panels [OSD_SCREEN_PANELS];
static uint8_t _count = 0;

void load (uint8_t num)
{
	if (num >= OSD_SCREENS) num = 0;

	uint8_t *offset = _eeprom_byte (OSD_SCREENS_EEPROM_OFFSET + num * sizeof (_panel_pos_t) * OSD_SCREEN_PANELS);

	uint8_t i = 0;
	for (; i < OSD_SCREEN_PANELS; i ++)
	{
		_panels [i].panel = eeprom_read_byte (offset);
		if (_panels [i].panel >= OSD_PANELS_COUNT)
			break;
		_panels [i].x = eeprom_read_byte (offset + 1);
		_panels [i].y = eeprom_read_byte (offset + 2);
		offset += sizeof (_panel_pos_t);
	}
	_count = i;

	max7456::clear ();
}

void draw ()
{
	max7456::wait_vsync ();
	max7456::clear ();
	for (uint8_t i = 0; i < _count; i ++)
		osd::panel::draw (_panels [i].panel, _panels [i].x, _panels [i].y);
}

void update ()
{
	for (uint8_t i = 0; i < _count; i ++)
		osd::panel::update (i);
}

namespace settings
{

const _panel_pos_t _default_screen_0 [] PROGMEM = {
	{OSD_PANEL_GPS_STATE, 1, 1},
	{OSD_PANEL_GPS_LAT, 8, 1},
	{OSD_PANEL_GPS_LON, 19, 1},

	{OSD_PANEL_CONNECTION_STATE, 0, 2},
	{OSD_PANEL_ARMING_STATE, 3, 2},
	{OSD_PANEL_FLIGHT_MODE, 24, 2},

	{OSD_PANEL_PITCH, 1, 6},
	{OSD_PANEL_HORIZON, 8, 6},
	{OSD_PANEL_THROTTLE, 23, 6},

	// {OSD_PANEL_GROUND_SPEED, 1, 8},
	{OSD_PANEL_ALT, 23, 8},

	{OSD_PANEL_ROLL, 1, 10},
	{OSD_PANEL_CLIMB, 23, 10},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 12},
	{OSD_PANEL_BATTERY_CURRENT, 22, 13},
	{OSD_PANEL_BATTERY_CONSUMED, 22, 14},

	{0xff, 0xff, 0xff}
};

#if OSD_SCREENS > 1
const _panel_pos_t _default_screen_1 [] PROGMEM = {
	{OSD_PANEL_CONNECTION_STATE, 0, 0},
	{OSD_PANEL_ARMING_STATE, 3, 0},
	{OSD_PANEL_FLIGHT_MODE, 24, 0},

	{OSD_PANEL_PITCH, 1, 6},
	{OSD_PANEL_ALT, 1, 8},
	{OSD_PANEL_ROLL, 1, 10},
	{OSD_PANEL_HORIZON, 8, 6},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 13},
	{OSD_PANEL_BATTERY_CURRENT, 22, 14},

	{0xff, 0xff, 0xff}
};
#endif

#if OSD_SCREENS > 2
const _panel_pos_t _default_screen_2 [] PROGMEM = {
	{OSD_PANEL_CONNECTION_STATE, 0, 0},
	{OSD_PANEL_ARMING_STATE, 3, 0},
	{OSD_PANEL_FLIGHT_MODE, 24, 0},

	{OSD_PANEL_PITCH, 1, 6},
	{OSD_PANEL_ALT, 1, 8},
	{OSD_PANEL_ROLL, 1, 10},

	{OSD_PANEL_FLIGHT_TIME, 1, 13},

	{OSD_PANEL_BATTERY_VOLTAGE, 22, 13},
	{OSD_PANEL_BATTERY_CURRENT, 22, 14},

	{0xff, 0xff, 0xff}
};
#endif


void _reset_screen (uint8_t num, const _panel_pos_t screen [], uint8_t len)
{
	uint8_t *offset = _eeprom_byte (OSD_SCREENS_EEPROM_OFFSET + num * sizeof (_panel_pos_t) * OSD_SCREEN_PANELS);
	for (uint8_t i = 0; i < len; i ++)
	{
		eeprom_update_byte (offset, pgm_read_byte (&(screen [i].panel)));
		eeprom_update_byte (offset + 1, pgm_read_byte (&(screen [i].x)));
		eeprom_update_byte (offset + 2, pgm_read_byte (&(screen [i].y)));
		offset += sizeof (_panel_pos_t);
	}
}

void reset ()
{
	_reset_screen (0, _default_screen_0, sizeof (_default_screen_0) / sizeof (_panel_pos_t));
#if OSD_SCREENS > 1
	_reset_screen (1, _default_screen_1, sizeof (_default_screen_1) / sizeof (_panel_pos_t));
#endif
#if OSD_SCREENS > 2
	_reset_screen (2, _default_screen_2, sizeof (_default_screen_2) / sizeof (_panel_pos_t));
#endif
}

}  // namespace settings

}  // namespace screen

}  // namespace osd

