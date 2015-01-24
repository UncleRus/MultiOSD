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

	bool flash = false;
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


namespace alt
{

	const char __name [] PROGMEM = "StableAlt";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x85%d\x8d"), (int16_t) telemetry::stable::altitude);
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

#define _PAN_GPS_2D 0x01
#define _PAN_GPS_3D 0x02

	void draw (uint8_t x, uint8_t y)
	{
		bool err = telemetry::gps::state == GPS_STATE_NO_FIX;
		max7456::puts_p (x, y, PSTR ("\x10\x11"), err ? MAX7456_ATTR_BLINK : 0);
		max7456::put (x + 2, y, telemetry::gps::state <  GPS_STATE_3D ? _PAN_GPS_2D : _PAN_GPS_3D,
			telemetry::gps::state < GPS_STATE_2D ? MAX7456_ATTR_BLINK : 0);
		if (err) max7456::puts_p (x + 3, y, PSTR ("ERR"), MAX7456_ATTR_BLINK);
		else
		{
			max7456::open (x + 3, y);
			fprintf_P (&max7456::stream, PSTR ("%d"), telemetry::gps::sattelites);
		}
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

	void draw (uint8_t x, uint8_t y)
	{
		uint8_t r = x + PANEL_HORIZON_WIDTH - 1;
		uint8_t c = y + PANEL_HORIZON_HEIGHT / 2;
		for (uint8_t i = y; i < y + PANEL_HORIZON_HEIGHT; i ++)
		{
			max7456::put (x, i, i == c ? PANEL_HORIZON_LEFT_CENTER : PANEL_HORIZON_LEFT_BORDER);
			max7456::put (r, i, i == c ? PANEL_HORIZON_RIGHT_CENTER : PANEL_HORIZON_RIGHT_BORDER);
		}

		// code below was taken from minoposd
		//int16_t pitch_line = round (tan (-_RADIAN * telemetry::attitude::pitch) * _PAN_HORZ_LINES);
		int16_t pitch_line = tan (-_RADIAN * telemetry::attitude::pitch) * _PAN_HORZ_LINES;
		//int32_t roll = tan (_RADIAN * telemetry::attitude::roll) * 1000000;
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
				//if (subval == 0) subval = 1;
				if (subval == 8) max7456::put (x + col, y + row - 2, PANEL_HORIZON_TOP);
				max7456::put (x + col, y + row - 1, PANEL_HORIZON_LINE + subval);
			}
		}
	}

}  // namespace horizon

namespace throttle
{

	const char __name [] PROGMEM = "Throttle";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x87%d%%"), telemetry::input::throttle);
	}

}  // namespace throttle

namespace ground_speed
{

	const char __name [] PROGMEM = "GroundSpeed";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\x12%d%\x8d"), telemetry::stable::ground_speed);
	}

}  // namespace ground_speed

namespace battery_voltage
{

	const char __name [] PROGMEM = "BatVoltage";

	void draw (uint8_t x, uint8_t y)
	{
		// TODO : change battery icon depending on voltage
		max7456::put (x, y,
			telemetry::messages::battery_low ? 0xf4 : 0xf7,
			telemetry::messages::battery_low ? MAX7456_ATTR_BLINK : 0
		);
		max7456::open (x + 1, y);
		fprintf_P (&max7456::stream, PSTR ("%.2f\x8e"), telemetry::battery::voltage);
	}

}  // namespace battery_voltage

namespace battery_current
{

	const char __name [] PROGMEM = "BatCurrent";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\xfa%.2f\x8f"), telemetry::battery::current);
	}

}  // namespace battery_current

namespace battery_consumed
{

	const char __name [] PROGMEM = "BatConsumed";

	void draw (uint8_t x, uint8_t y)
	{
		max7456::open (x, y);
		fprintf_P (&max7456::stream, PSTR ("\xfb%u\x82"), (uint16_t) telemetry::battery::consumed);
	}

}  // namespace battery_consumed

}  // namespace __panels

namespace panel
{

const panel_draw_t panels [] PROGMEM = {
	osd::__panels::alt::draw,
	osd::__panels::climb::draw,
	osd::__panels::flight_mode::draw,
	osd::__panels::arming_state::draw,
	osd::__panels::connection_state::draw,
	osd::__panels::flight_time::draw,
	osd::__panels::roll::draw,
	osd::__panels::pitch::draw,
	osd::__panels::gps_state::draw,
	osd::__panels::gps_lat::draw,
	osd::__panels::gps_lon::draw,
	osd::__panels::horizon::draw,
	osd::__panels::throttle::draw,
	osd::__panels::ground_speed::draw,
	osd::__panels::battery_voltage::draw,
	osd::__panels::battery_current::draw,
	osd::__panels::battery_consumed::draw,
};

const char * const panel_names [] PROGMEM = {
	osd::__panels::alt::__name,
	osd::__panels::climb::__name,
	osd::__panels::flight_mode::__name,
	osd::__panels::arming_state::__name,
	osd::__panels::connection_state::__name,
	osd::__panels::flight_time::__name,
	osd::__panels::roll::__name,
	osd::__panels::pitch::__name,
	osd::__panels::gps_state::__name,
	osd::__panels::gps_lat::__name,
	osd::__panels::gps_lon::__name,
	osd::__panels::horizon::__name,
	osd::__panels::throttle::__name,
	osd::__panels::ground_speed::__name,
	osd::__panels::battery_voltage::__name,
	osd::__panels::battery_current::__name,
	osd::__panels::battery_consumed::__name,
};

void draw (uint8_t panel, uint8_t x, uint8_t y)
{
	((panel_draw_t) pgm_read_word (&panels [panel])) (x, y);
}

}  // namespace panels

}  // namespace osd
