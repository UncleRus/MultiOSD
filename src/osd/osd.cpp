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
#include "osd.h"
#include "../settings.h"
#include "../telemetry/telemetry.h"
#include "../config.h"
#include "../lib/timer/timer.h"
#include "../lib/max7456/max7456.h"

namespace osd
{

#define OSD_EEPROM_SWITCH             _eeprom_byte (OSD_EEPROM_OFFSET)
#define OSD_EEPROM_SWITCH_RAW_CHANNEL _eeprom_byte (OSD_EEPROM_OFFSET + 1)
#define OSD_EEPROM_SCREENS            _eeprom_byte (OSD_EEPROM_OFFSET + 2)

#ifndef OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT
#	define OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT 6
#endif

static uint8_t _switch;
static uint8_t _channel;
static uint8_t _screen;
static uint8_t _screens_enabled;
static uint8_t _raw_lvl_size;
static bool _visible;

#if (OSD_MAX_SCREENS <= 0) || (OSD_MAX_SCREENS > 8)
#	error OSD_MAX_SCREENS must be > 0 and < 9
#endif

#if OSD_MAX_SCREENS > 1

#define _OSD_RAW_LVL_RANGE (OSD_RAW_CHANNEL_MAX - OSD_RAW_CHANNEL_MIN)

uint8_t _get_screen (uint16_t raw)
{
	if (raw < OSD_RAW_CHANNEL_MIN) raw = OSD_RAW_CHANNEL_MIN;
	if (raw > OSD_RAW_CHANNEL_MAX) raw = OSD_RAW_CHANNEL_MAX;
	return (raw - OSD_RAW_CHANNEL_MIN) / _raw_lvl_size;
}

bool _check_input ()
{
	if (_switch == OSD_SWITCH_OFF || !telemetry::input::connected) return false;

	uint8_t old_screen = _screen;

	_screen = _switch == OSD_SWITCH_FLIGHT_MODE
		? telemetry::input::flight_mode_switch
		: _get_screen (telemetry::input::channels [_channel]);

	if (_screen >= _screens_enabled) _screen = _screens_enabled - 1;

	return _screen != old_screen;
}
#endif

void main ()
{
	_visible = true;
	screen::load (_screen);

	while (true)
	{
		bool updated = telemetry::update ();
#if OSD_MAX_SCREENS > 1
		if (_screens_enabled > 1 && _check_input ())
		{
			screen::load (_screen);
			updated = true;
		}
#endif
		if (updated)
		{
			screen::update ();
			if (_visible) screen::draw ();
		}
	}
}

void init ()
{
	_switch = eeprom_read_byte (OSD_EEPROM_SWITCH);
	_channel = eeprom_read_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL);
	_screens_enabled = eeprom_read_byte (OSD_EEPROM_SCREENS);
	if (_screens_enabled )
	_raw_lvl_size = _OSD_RAW_LVL_RANGE / _screens_enabled;
}

namespace settings
{

	void reset ()
	{
		eeprom_update_byte (OSD_EEPROM_SWITCH, OSD_EEPROM_SWITCH_DEFAULT);
		eeprom_update_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL, OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT);
		eeprom_update_byte (OSD_EEPROM_SCREENS, OSD_MAX_SCREENS);
		screen::settings::reset ();
	}

}  // namespace settings

}  // namespace osd
