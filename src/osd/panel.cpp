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
#include <math.h>

namespace osd
{

namespace draw
{

	/*
	 * 012
	 * 3 7
	 * 456
	 */
	const uint8_t _rect_thin [] PROGMEM = {0xd0, 0xd1, 0xd2, 0xd3, 0xd4, 0xd5, 0xd6, 0xd7};
	const uint8_t _rect_fill [] PROGMEM = {0xd8, 0xd9, 0xda, 0xdb, 0xdc, 0xdd, 0xde, 0xdf};

#define _get_rect_char(x) (pgm_read_byte (&_rect [x]))

	void rect (uint8_t l, uint8_t t, uint8_t w, uint8_t h, bool filled, uint8_t attr)
	{
		uint8_t r = l + w - 1;
		uint8_t b = t + h - 1;

		const uint8_t *_rect = filled ? _rect_fill : _rect_thin;

		max7456::put (l, t, _get_rect_char (0), attr);
		max7456::put (r, t, _get_rect_char (2), attr);
		max7456::put (l, b, _get_rect_char (4), attr);
		max7456::put (r, b, _get_rect_char (6), attr);

		if (w > 2)
			for (uint8_t i = 1; i < w - 1; i ++)
			{
				max7456::put (l + i, t, _get_rect_char (1), attr);
				max7456::put (l + i, b, _get_rect_char (5), attr);
			}

		if (h > 2)
			for (uint8_t i = 1; i < h - 1; i ++)
			{
				max7456::put (l, t + i, _get_rect_char (3), attr);
				max7456::put (r, t + i, _get_rect_char (7), attr);
			}
	}

}  // namespace draw


namespace __panels
{

#define STD_DRAW void draw (uint8_t x, uint8_t y) \
{ \
	max7456::puts (x, y, _buffer); \
}


namespace alt
{

	const char __name [] PROGMEM = "StableAlt";

	static char _buffer [8];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\x85%d\x8d"), (int16_t) telemetry::stable::altitude);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW;

}  // namespace alt

namespace climb
{

	const char __name [] PROGMEM = "Climb";

	static char _buffer [8];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("%c%.1f\x8c"),
			telemetry::stable::climb < 0 ? 0x07 : 0x08, telemetry::stable::climb);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

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

	static const char *name;
	static uint8_t len;

	void update ()
	{
		name = (const char *) pgm_read_ptr (&_fm [telemetry::status::flight_mode]);
		len = strlen_P (name);
	}

	void draw (uint8_t x, uint8_t y)
	{
		osd::draw::rect (x, y, len + 2, 3);
		max7456::puts_p (x + 1, y + 1, name);
	}

}  // namespace flight_mode

namespace arming_state
{

	const char __name [] PROGMEM = "ArmState";

	void update () {}

	void draw (uint8_t x, uint8_t y)
	{
		osd::draw::rect (x, y, 3, 3, telemetry::status::armed);
		max7456::put (x + 1, y + 1, telemetry::status::armed ? 0xe0 : 'A');
	}

}  // namespace name

namespace connection_state
{

	const char __name [] PROGMEM = "ConState";

	void update () {}

	void draw (uint8_t x, uint8_t y)
	{
		uint8_t attr = telemetry::status::connection != CONNECTION_STATE_CONNECTED
			? MAX7456_ATTR_INVERT
			: 0;

		osd::draw::rect (x, y, 3, 3, true, attr);
		max7456::put (x + 1, y + 1, 0xe1, attr);
	}

}  // namespace connection_state

namespace flight_time
{

	const char __name [] PROGMEM = "FlightTime";

	static char _buffer [8];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\xb3%02u:%02u"), telemetry::status::flight_time / 60, telemetry::status::flight_time % 60);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace flight_time

namespace roll
{

	const char __name [] PROGMEM = "Roll";

	static char _buffer [7];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\xb2%d\xb0"), (int16_t) telemetry::attitude::roll);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace roll

namespace pitch
{

	const char __name [] PROGMEM = "Pitch";

	static char _buffer [7];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\xb1%d\xb0"), (int16_t) telemetry::attitude::pitch);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace pitch

namespace gps_state
{

	const char __name [] PROGMEM = "GPS";

#define _PAN_GPS_2D 0x01
#define _PAN_GPS_3D 0x02

	static char _buffer [4];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("%d"), telemetry::gps::sattelites);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	void draw (uint8_t x, uint8_t y)
	{
		bool err = telemetry::gps::state == GPS_STATE_NO_FIX;
		max7456::puts_p (x, y, PSTR ("\x10\x11"), err ? MAX7456_ATTR_BLINK : 0);
		max7456::put (x + 2, y, telemetry::gps::state <  GPS_STATE_3D ? _PAN_GPS_2D : _PAN_GPS_3D,
			telemetry::gps::state < GPS_STATE_2D ? MAX7456_ATTR_BLINK : 0);
		if (err) max7456::puts_p (x + 3, y, PSTR ("ERR"), MAX7456_ATTR_BLINK);
		else max7456::puts (x + 3, y, _buffer);
	}

}  // namespace gps_state

namespace gps_lat
{

	const char __name [] PROGMEM = "Lat";

	static char _buffer [11];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\x83%02.6f"), telemetry::gps::latitude);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace gps_lat

namespace gps_lon
{

	const char __name [] PROGMEM = "Lon";

	static char _buffer [11];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\x84%02.6f"), telemetry::gps::longitude);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace gps_lon

namespace horizon
{

#define PANEL_HORIZON_WIDTH 14
#define PANEL_HORIZON_HEIGHT 5

#define PANEL_HORIZON_LEFT_BORDER 0xb8
#define PANEL_HORIZON_LEFT_CENTER 0xc8
#define PANEL_HORIZON_RIGHT_BORDER 0xb9
#define PANEL_HORIZON_RIGHT_CENTER 0xc9
#define PANEL_HORIZON_LINE 0x16
#define PANEL_HORIZON_TOP 0x0e

#define _PAN_HORZ_CHAR_LINES 18
#define _PAN_HORZ_VRES 9
#define _PAN_HORZ_INT_WIDTH (PANEL_HORIZON_WIDTH - 2)
#define _PAN_HORZ_LINES (PANEL_HORIZON_HEIGHT * _PAN_HORZ_VRES)
#define _PAN_HORZ_TOTAL_LINES (PANEL_HORIZON_HEIGHT * _PAN_HORZ_CHAR_LINES)

#define _RADIAN 0.017453293

	const char __name [] PROGMEM = "Horizon";

	uint8_t _buffer [_PAN_HORZ_INT_WIDTH];

	void update ()
	{
		// code below was taken from minoposd
		int16_t pitch_line = tan (-_RADIAN * telemetry::attitude::pitch) * _PAN_HORZ_LINES;
		float roll = tan (_RADIAN * telemetry::attitude::roll);
		for (uint8_t col = 1; col <= _PAN_HORZ_INT_WIDTH; col ++)
		{
			// center X point at middle of each column
			int16_t middle = col * _PAN_HORZ_INT_WIDTH - (_PAN_HORZ_INT_WIDTH * _PAN_HORZ_INT_WIDTH / 2) - _PAN_HORZ_INT_WIDTH / 2;
			// calculating hit point on Y plus offset to eliminate negative values
			//int8_t hit = roll * middle / 1000000 + pitch_line + _PAN_HORZ_LINES + 2;
			int8_t hit = roll * middle + pitch_line + _PAN_HORZ_LINES;
			if (hit > 0 && hit < _PAN_HORZ_TOTAL_LINES)
			{
				int8_t row = PANEL_HORIZON_HEIGHT - ((hit - 1) / _PAN_HORZ_CHAR_LINES);
				int8_t subval = (hit - (_PAN_HORZ_TOTAL_LINES - row * _PAN_HORZ_CHAR_LINES + 1)) * _PAN_HORZ_VRES / _PAN_HORZ_CHAR_LINES ;
				_buffer [col - 1] = (row - 1) << 4 | subval;
			}
			else _buffer [col - 1] = 0xff;
		}
	}

	void draw (uint8_t x, uint8_t y)
	{
		uint8_t r = x + PANEL_HORIZON_WIDTH - 1;
		uint8_t c = y + PANEL_HORIZON_HEIGHT / 2;
		for (uint8_t i = y; i < y + PANEL_HORIZON_HEIGHT; i ++)
		{
			max7456::put (x, i, i == c ? PANEL_HORIZON_LEFT_CENTER : PANEL_HORIZON_LEFT_BORDER);
			max7456::put (r, i, i == c ? PANEL_HORIZON_RIGHT_CENTER : PANEL_HORIZON_RIGHT_BORDER);
		}

		for (uint8_t i = 0; i < _PAN_HORZ_INT_WIDTH; i ++)
		{
			if (_buffer [i] == 0xff) continue;

			uint8_t row = _buffer [i] >> 4;
			uint8_t subval = _buffer [i] & 0x0f;

			if (subval == _PAN_HORZ_VRES - 1) max7456::put (x + i + 1, y + row - 1, PANEL_HORIZON_TOP);
			max7456::put (x + i + 1, y + row, PANEL_HORIZON_LINE + subval);
		}
	}

}  // namespace horizon

namespace throttle
{

	const char __name [] PROGMEM = "Throttle";

	static char _buffer [7];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\x87%d%%"), telemetry::input::throttle);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace throttle

namespace ground_speed
{

	const char __name [] PROGMEM = "GroundSpeed";

	static char _buffer [7];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\x0a%d\x81"), (int16_t) (telemetry::stable::ground_speed * 3.6));
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace ground_speed

namespace battery_voltage
{

	const char __name [] PROGMEM = "BatVoltage";

	static char _buffer [7];
	static char _symbol;
	static uint8_t _attr;

	void update ()
	{
		sprintf_P (_buffer, PSTR ("%.2f\x8e"), telemetry::battery::voltage);
		_buffer [sizeof (_buffer) - 1] = 0;

		_symbol = 0xf4 + (uint8_t) round (telemetry::battery::level / 20.0);
		_attr = telemetry::messages::battery_low ? MAX7456_ATTR_BLINK : 0;
	}

	void draw (uint8_t x, uint8_t y)
	{
		max7456::put (x, y, _symbol, _attr);
		max7456::puts (x + 1, y, _buffer);
	}

}  // namespace battery_voltage

namespace battery_current
{

	const char __name [] PROGMEM = "BatCurrent";

	static char _buffer [8];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\xfa%.2f\x8f"), telemetry::battery::current);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace battery_current

namespace battery_consumed
{

	const char __name [] PROGMEM = "BatConsumed";

	static char _buffer [8];

	void update ()
	{
		sprintf_P (_buffer, PSTR ("\xfb%u\x82"), (uint16_t) telemetry::battery::consumed);
		_buffer [sizeof (_buffer) - 1] = 0;
	}

	STD_DRAW

}  // namespace battery_consumed

namespace rssi_flag
{

	const char __name [] PROGMEM = "RSSIFlag";

	void update () {}

	void draw (uint8_t x, uint8_t y)
	{
		if (telemetry::messages::rssi_low) max7456::put (x, y, 0xb4, MAX7456_ATTR_BLINK);
	}

}  // namespace rssi_flag

namespace home_distance
{

	const char __name [] PROGMEM = "HomeDistance";

	static char _buffer [8];
	static uint8_t _attr, _i_attr;

	void update ()
	{
		_attr = telemetry::home::state == HOME_STATE_NO_FIX ? MAX7456_ATTR_BLINK : 0;
		_i_attr = telemetry::home::state != HOME_STATE_FIXED ? MAX7456_ATTR_BLINK : 0;
		if (_i_attr)
		{
			sprintf_P (_buffer, PSTR ("%S"), telemetry::home::state == HOME_STATE_NO_FIX ? PSTR ("ERR") : PSTR ("\x09\x09\x09\x8d"));
			return;
		}
		if (telemetry::home::distance >= 10000)
			 sprintf_P (_buffer, PSTR ("%.1f\x8b"), telemetry::home::distance / 1000);
		else sprintf_P (_buffer, PSTR ("%u\x8d"), (uint16_t) telemetry::home::distance);
	}

	void draw (uint8_t x, uint8_t y)
	{
		max7456::put (x, y, 0x12, _i_attr);
		max7456::puts (x + 1, y, _buffer, _attr);
	}

}  // namespace home_distance

namespace home_direction
{

#define _PAN_HD_ARROWS 0x90

	const char __name [] PROGMEM = "HomeDirection";

	static uint8_t _arrow;

	void update ()
	{
		_arrow = _PAN_HD_ARROWS + telemetry::home::direction * 2;
	}

	void draw (uint8_t x, uint8_t y)
	{
		if (telemetry::home::state != HOME_STATE_FIXED) return;
		max7456::put (x, y, _arrow);
		max7456::put (x + 1, y, _arrow + 1);
	}

}  // namespace home_direction

}  // namespace __panels

namespace panel
{

#define _declare_panel(NS) { osd::__panels:: NS ::__name, osd::__panels:: NS ::update, osd::__panels:: NS ::draw }

const panel_t panels [] PROGMEM = {
	_declare_panel (alt),
	_declare_panel (climb),
	_declare_panel (flight_mode),
	_declare_panel (arming_state),
	_declare_panel (connection_state),
	_declare_panel (flight_time),
	_declare_panel (roll),
	_declare_panel (pitch),
	_declare_panel (gps_state),
	_declare_panel (gps_lat),
	_declare_panel (gps_lon),
	_declare_panel (horizon),
	_declare_panel (throttle),
	_declare_panel (ground_speed),
	_declare_panel (battery_voltage),
	_declare_panel (battery_current),
	_declare_panel (battery_consumed),
	_declare_panel (rssi_flag),
	_declare_panel (home_distance),
	_declare_panel (home_direction),
};

const uint8_t count = sizeof (panels) / sizeof (panel_t);

}  // namespace panels

}  // namespace osd
