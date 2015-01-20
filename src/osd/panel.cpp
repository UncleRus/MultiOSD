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

#include "panel.h"
#include <avr/pgmspace.h>
#include "../lib/max7456/max7456.h"
#include "../telemetry/telemetry.h"

namespace osd
{

namespace draw
{

	bool flash = false;
	/*
	 * 012
	 * 3 7
	 * 456
	 */
	const uint8_t _rect_norm [] PROGMEM = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7};
	const uint8_t _rect_inv [] PROGMEM = {0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf};

#define _get_rect_char(x) (pgm_read_byte (&_rect [x]))

	void rect (uint8_t l, uint8_t t, uint8_t w, uint8_t h, bool inv)
	{
		uint8_t r = l + w - 1;
		uint8_t b = t + h - 1;

		const uint8_t *_rect = inv ? _rect_inv : _rect_norm;

		max7456::put (l, t, _get_rect_char (0));
		max7456::put (r, t, _get_rect_char (2));
		max7456::put (l, b, _get_rect_char (4));
		max7456::put (r, b, _get_rect_char (6));

		if (w > 2)
			for (uint8_t i = 1; i < w - 1; i ++)
			{
				max7456::put (l + i, t, _get_rect_char (1));
				max7456::put (l + i, b, _get_rect_char (5));
			}

		if (h > 2)
			for (uint8_t i = 1; i < h - 1; i ++)
			{
				max7456::put (l, t + i, _get_rect_char (3));
				max7456::put (r, t + i, _get_rect_char (7));
			}
	}

}  // namespace draw


namespace panel
{


namespace alt
{

	const char __name [] PROGMEM = "StableAlt";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x85%.1f\x8d"), telemetry::stable::altitude);
	}

}  // namespace alt

namespace climb
{

	const char __name [] PROGMEM = "Climb";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("%c%.1f\x8c"),
			telemetry::stable::climb < 0 ? 0x07 : 0x08, telemetry::stable::climb);
	}

}  // namespace climb_rate

namespace flight_mode
{

	const char __name [] PROGMEM = "FlightMode";

	const char _fm_man [] PROGMEM = "MANU";
	const char _fm_stab1 [] PROGMEM = "STB1";
	const char _fm_stab2 [] PROGMEM = "STB2";
	const char _fm_stab3 [] PROGMEM = "STB3";
	const char _fm_stab4 [] PROGMEM = "STB4";
	const char _fm_stab5 [] PROGMEM = "STB5";
	const char _fm_stab6 [] PROGMEM = "STB6";
	const char _fm_atune [] PROGMEM = "ATUN";
	const char _fm_pos_hold [] PROGMEM = "PHLD";
	const char _fm_pos_v_fpv [] PROGMEM = "PVAF";
	const char _fm_pos_v_los [] PROGMEM = "PVAL";
	const char _fm_pos_v_nsew [] PROGMEM = "PVAN";
	const char _fm_rtb [] PROGMEM = "RTH ";
	const char _fm_land [] PROGMEM = "LAND";
	const char _fm_plan [] PROGMEM = "PLAN";
	const char _fm_poi [] PROGMEM = "POI ";
	const char _fm_acruise [] PROGMEM = "ACRU";

	const char * const _fm [] PROGMEM = {
		_fm_man, _fm_stab1, _fm_stab2, _fm_stab3, _fm_stab4, _fm_stab5, _fm_stab6,
		_fm_atune, _fm_pos_hold, _fm_pos_v_fpv, _fm_pos_v_los, _fm_pos_v_nsew,
		_fm_rtb, _fm_land, _fm_plan, _fm_poi, _fm_acruise
	};

	void draw (uint8_t x, uint8_t y)
	{
		const char *name = (const char *) pgm_read_word (&_fm [telemetry::status::flight_mode]);
		osd::draw::rect (x, y, strlen_P (name) + 2, 3);
		max7456::puts_p (x + 1, y + 1, name);
	}

}  // namespace flight_mode

namespace arming_state
{

	const char __name [] PROGMEM = "ArmState";

	void draw (uint8_t x, uint8_t y)
	{
		osd::draw::rect (x, y, 3, 3, telemetry::status::armed);
		max7456::put (x + 1, y + 1, telemetry::status::armed ? 0xe0 : 'A');
	}

}  // namespace name

namespace connection_state
{

	const char __name [] PROGMEM = "ConState";

	void draw (uint8_t x, uint8_t y)
	{
		bool connected = telemetry::status::connection == CONNECTION_STATE_CONNECTED;
		osd::draw::rect (x, y, 3, 3, connected);
		max7456::put (x + 1, y + 1, connected ? 0xe1 : 'C');
	}

}  // namespace connection_state

namespace flight_time
{

	const char __name [] PROGMEM = "FlightTime";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\xb3%02u:%02u"), telemetry::status::flight_time / 60, telemetry::status::flight_time % 60);
	}

}  // namespace flight_time

namespace roll
{

	const char __name [] PROGMEM = "Roll";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\xb2%d\xb0"), (int16_t) telemetry::attitude::roll);
	}

}  // namespace roll

namespace pitch
{

	const char __name [] PROGMEM = "Pitch";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\xb1%d\xb0"), (int16_t) telemetry::attitude::pitch);
	}

}  // namespace pitch


namespace gps_state
{

	const char __name [] PROGMEM = "GPS";

	const uint8_t _states [] PROGMEM = {0x09, 0x09, 0x01, 0x02};

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x10\x11%c%d"),
			pgm_read_byte (&_states [telemetry::gps::state]), telemetry::gps::sattelites);
	}

}  // namespace gps_state

namespace gps_lat
{

	const char __name [] PROGMEM = "Lat";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x83%02.6f"), telemetry::gps::latitude);
	}

}  // namespace gps_lat

namespace gps_lon
{

	const char __name [] PROGMEM = "Lon";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x84%02.6f"), telemetry::gps::latitude);
	}

}  // namespace gps_lon

}  // namespace panel

const panel_draw_t panels [] PROGMEM = {
	osd::panel::alt::draw,
	osd::panel::climb::draw,
	osd::panel::flight_mode::draw,
	osd::panel::arming_state::draw,
	osd::panel::connection_state::draw,
	osd::panel::flight_time::draw,
	osd::panel::roll::draw,
	osd::panel::pitch::draw,
	osd::panel::gps_state::draw,
	osd::panel::gps_lat::draw,
	osd::panel::gps_lon::draw
};

const char * const panel_names [] PROGMEM = {
	osd::panel::alt::__name,
	osd::panel::climb::__name,
	osd::panel::flight_mode::__name,
	osd::panel::arming_state::__name,
	osd::panel::connection_state::__name,
	osd::panel::flight_time::__name,
	osd::panel::roll::__name,
	osd::panel::pitch::__name,
	osd::panel::gps_lat::__name,
	osd::panel::gps_lon::__name
};

void draw_panel (uint8_t panel, uint8_t x, uint8_t y)
{
	panel_draw_t func = (panel_draw_t) pgm_read_word (&panels [panel]);
	func (x, y);
}

}  // namespace panel
