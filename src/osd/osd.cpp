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

#ifndef OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT
#	define OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT 6
#endif

static uint8_t _switch;
static uint8_t _channel;
static uint8_t _screen;
static bool _visible;

#if OSD_SCREENS > 1

#define _OSD_RAW_LVL_RANGE (OSD_RAW_CHANNEL_MAX - OSD_RAW_CHANNEL_MIN)
#define _OSD_RAW_LVL_SIZE (_OSD_RAW_LVL_RANGE / OSD_SCREENS)

uint8_t _get_screen (uint16_t raw)
{
	if (raw < OSD_RAW_CHANNEL_MIN) raw = OSD_RAW_CHANNEL_MIN;
	if (raw > OSD_RAW_CHANNEL_MAX) raw = OSD_RAW_CHANNEL_MAX;
	return (raw - OSD_RAW_CHANNEL_MIN) / _OSD_RAW_LVL_SIZE;
}

bool _check_input ()
{
	if (_switch == OSD_SWITCH_OFF || !telemetry::input::connected) return false;

	uint8_t old_screen = _screen;

	_screen = _switch == OSD_SWITCH_FLIGHT_MODE
		? telemetry::input::flight_mode_switch
		: _get_screen (telemetry::input::channels [_channel]);

	if (_screen >= OSD_SCREENS) _screen = OSD_SCREENS - 1;

	return _screen != old_screen;
}
#endif

void main ()
{
	max7456::clear ();

	uint8_t draw_interval = max7456::mode == MAX7456_MODE_PAL ? OSD_DRAW_INTERVAL_PAL : OSD_DRAW_INTERVAL_NTSC;
	uint32_t draw_time = 0;

	_visible = true;
	screen::load (_screen);

	while (true)
	{
		uint32_t ticks = timer::ticks ();
		bool updated = telemetry::update ();
#if OSD_SCREENS > 1
		if (_check_input ())
		{
			screen::load (_screen);
			updated = true;
		}
#endif
		if (_visible && updated && ticks >= draw_time)
		{
			draw_time = ticks + draw_interval;
			screen::draw ();
		}
	}
}

void init ()
{
	_switch = eeprom_read_byte (OSD_EEPROM_SWITCH);
	_channel = eeprom_read_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL);
}

namespace settings
{

	void reset ()
	{
		eeprom_update_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL, OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT);
		eeprom_update_byte (OSD_EEPROM_SWITCH, OSD_EEPROM_SWITCH_DEFAULT);
		screen::settings::reset ();
	}

}  // namespace settings

}  // namespace osd


