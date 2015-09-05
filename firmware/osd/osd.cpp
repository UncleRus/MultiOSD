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
#include <avr/interrupt.h>
#include <avr/io.h>
#include <avr/wdt.h>

#include "../lib/max7456/max7456.h"

namespace osd
{

#define OSD_EEPROM_SWITCH             _eeprom_byte (OSD_EEPROM_OFFSET)
#define OSD_EEPROM_SWITCH_RAW_CHANNEL _eeprom_byte (OSD_EEPROM_OFFSET + 1)
#define OSD_EEPROM_SCREENS            _eeprom_byte (OSD_EEPROM_OFFSET + 2)
#define OSD_EEPROM_CHANNEL_MIN        _eeprom_word (OSD_EEPROM_OFFSET + 3)
#define OSD_EEPROM_CHANNEL_MAX        _eeprom_word (OSD_EEPROM_OFFSET + 5)

#ifndef OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT
#	define OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT 6
#endif

uint8_t _switch;
uint8_t _channel;
uint16_t _chan_min, _chan_max, _raw_lvl_size;
uint8_t cur_screen;
uint8_t _screens_enabled;
//bool visible;

#if (OSD_MAX_SCREENS <= 0) || (OSD_MAX_SCREENS > 8)
#	error OSD_MAX_SCREENS must be between 0 and 8
#endif

#if OSD_MAX_SCREENS > 1

uint8_t get_screen (uint16_t raw)
{
	if (raw < _chan_min) raw = _chan_min;
	if (raw > _chan_max) raw = _chan_max;
	return (raw - _chan_min) / _raw_lvl_size;
}

bool check_input ()
{
	if (_switch == OSD_SWITCH_OFF || !telemetry::input::connected) return false;

	uint8_t old_screen = cur_screen;

	cur_screen = _switch == OSD_SWITCH_FLIGHT_MODE
		? telemetry::input::flight_mode_switch
		: get_screen (telemetry::input::channels [_channel]);

	if (cur_screen >= _screens_enabled) cur_screen = _screens_enabled - 1;

	return cur_screen != old_screen;
}

#endif

volatile bool started = false;
volatile bool mutex = false;

ISR (INT0_vect, ISR_NOBLOCK)
{
	if (!started || !screen::updated || mutex) return;
	mutex = true;

	screen::draw ();

	max7456::open (1, 1);
	fprintf_P (&max7456::stream, PSTR (" %u "), cur_screen);

	mutex = false;
}

void main ()
{
	// TODO: hide and show
	// visible = true;

	screen::load (0);
	screen::update ();

	// VSYNC external interrupt
	EICRA = _BV (ISC01);	// VSYNC falling edge
	EIMSK = _BV (INT0);		// enable
	started = true;

	// just in case
	wdt_enable (WDTO_250MS);

	while (true)
	{
		wdt_reset ();

		bool updated = telemetry::update ();
#if OSD_MAX_SCREENS > 1
		if (_screens_enabled > 1 && check_input ())
		{
			screen::load (cur_screen);
			updated = true;
		}
#endif
		//if (updated && visible)
		if (updated)
		{
			while (mutex)
				;
			mutex = true;
			screen::update ();
			mutex = false;
		}
	}
}

void init ()
{
	_switch = eeprom_read_byte (OSD_EEPROM_SWITCH);
	_channel = eeprom_read_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL);
	_screens_enabled = eeprom_read_byte (OSD_EEPROM_SCREENS);
	_chan_min = eeprom_read_word (OSD_EEPROM_CHANNEL_MIN);
	_chan_max = eeprom_read_word (OSD_EEPROM_CHANNEL_MAX);
	if (_screens_enabled > OSD_MAX_SCREENS) _screens_enabled = OSD_MAX_SCREENS;
	_raw_lvl_size = (_chan_max - _chan_min) / _screens_enabled;
}

namespace settings
{

	void reset ()
	{
		eeprom_update_byte (OSD_EEPROM_SWITCH, OSD_EEPROM_SWITCH_DEFAULT);
		eeprom_update_byte (OSD_EEPROM_SWITCH_RAW_CHANNEL, OSD_EEPROM_SWITCH_RAW_CHANNEL_DEFAULT);
		eeprom_update_byte (OSD_EEPROM_SCREENS, OSD_MAX_SCREENS);
		eeprom_update_word (OSD_EEPROM_CHANNEL_MIN, OSD_CHANNEL_MIN);
		eeprom_update_word (OSD_EEPROM_CHANNEL_MAX, OSD_CHANNEL_MAX);
		screen::settings::reset ();
	}

}  // namespace settings

}  // namespace osd
