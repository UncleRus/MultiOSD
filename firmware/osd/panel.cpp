/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * MultiOSD is free software: you can redistribute it and/or modify
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
#include "../lib/max7456/max7456.h"
#include "../telemetry/telemetry.h"
#include "symbols.h"
#include <math.h>
#include <string.h>

namespace osd
{

namespace draw
{

	/*
	 * 012
	 * 3 7
	 * 456
	 */
	const uint8_t _rect_thin[] PROGMEM = {
        OSD_CHAR_RECT_THIN_LT, OSD_CHAR_RECT_THIN_CT, OSD_CHAR_RECT_THIN_RT,
        OSD_CHAR_RECT_THIN_LC, OSD_CHAR_RECT_THIN_LB, OSD_CHAR_RECT_THIN_CB,
        OSD_CHAR_RECT_THIN_RB, OSD_CHAR_RECT_THIN_RC
	};
	const uint8_t _rect_fill[] PROGMEM = {
        OSD_CHAR_RECT_FILL_LT, OSD_CHAR_RECT_FILL_CT, OSD_CHAR_RECT_FILL_RT,
        OSD_CHAR_RECT_FILL_LC, OSD_CHAR_RECT_FILL_LB, OSD_CHAR_RECT_FILL_CB,
        OSD_CHAR_RECT_FILL_RB, OSD_CHAR_RECT_FILL_RC
    };

    void rect(uint8_t l, uint8_t t, uint8_t w, uint8_t h, bool filled, uint8_t attr)
    {
        if (w < 2 || h < 2)
            return;

        uint8_t r = w - 1;
        uint8_t b = h - 1;

        char _rect[8];
        memcpy_P(_rect, filled ? _rect_fill : _rect_thin, 8);
        char buffer[w + 1];

        for (uint8_t i = 0; i < h; i++)
        {
            char spacer;
            if (i == 0)
            {
                buffer[0] = _rect[0];
                buffer[r] = _rect[2];
                spacer = _rect[1];
            }
            else if (i == b)
            {
                buffer[0] = _rect[4];
                buffer[r] = _rect[6];
                spacer = _rect[5];
            }
            else
            {
                buffer[0] = _rect[3];
                buffer[r] = _rect[7];
                spacer = ' ';
            }
            if (w > 2)
                memset(buffer + 1, spacer, w - 2);
            buffer[w] = 0;
            max7456::puts(l, t + i, buffer, attr);
        }
    }

    void arrow(uint8_t x, uint8_t y, uint16_t direction, uint8_t attr)
    {
        uint8_t chr = OSD_CHAR_ARROWS + ((uint8_t) (direction / 360.0 * 16)) * 2;
        max7456::put(x, y, chr, attr);
        max7456::put(x + 1, y, chr + 1, attr);
    }

    void battery_voltage(uint8_t x, uint8_t y, uint8_t symbol_offset, telemetry::battery::battery_t *bat)
    {
        max7456::put(x, y, symbol_offset + (bat->level + 10) / 20, bat->low ? MAX7456_ATTR_BLINK : 0);
        max7456::open(x + 1, y);
        fprintf_P(&max7456::stream, PSTR("%.2f" OSD_SYMBOL_V), bat->voltage);
    }

}  // namespace draw


namespace panels
{

#define terminate_buffer() do { buffer[sizeof(buffer) - 1] = 0; } while (0)

#define DECLARE_BUF(n) char buffer[n];

#define PANEL_NAME(__name) const char name[] PROGMEM = __name;

#define STD_DRAW void draw(uint8_t x, uint8_t y) \
{ \
	max7456::puts(x, y, buffer); \
}

#define STD_UPDATE(fmt, ...) void update() \
{ \
	snprintf_P(buffer, sizeof(buffer), PSTR(fmt), __VA_ARGS__); \
	terminate_buffer(); \
}

#define STD_PANEL(__name, bs, fmt, ...) \
	PANEL_NAME(__name); \
	DECLARE_BUF(bs); \
	STD_UPDATE(fmt, __VA_ARGS__); \
	STD_DRAW;


namespace alt
{

    STD_PANEL("StableAlt", 8, OSD_SYMBOL_ALT "%d" OSD_SYMBOL_M, (int16_t ) round(telemetry::stable::altitude));
	//STD_PANEL("StableAlt", 8, OSD_SYMBOL_ALT "%.1f" OSD_SYMBOL_M, telemetry::stable::altitude);

}  // namespace alt

namespace climb
{

    PANEL_NAME("Climb");

    DECLARE_BUF(8);

    void update()
    {
        int8_t c = round(telemetry::stable::climb * 10);
        uint8_t s;
        if (c >= 20)
            s = OSD_CHAR_CLIMB3;
        else if (c >= 10)
            s = OSD_CHAR_CLIMB2;
        else if (c >= 0)
            s = OSD_CHAR_CLIMB1;
        else if (c <= -20)
            s = OSD_CHAR_FALL3;
        else if (c <= -10)
            s = OSD_CHAR_FALL2;
        else
            s = OSD_CHAR_FALL1;
        snprintf_P(buffer, sizeof(buffer), PSTR("%c%.1f" OSD_SYMBOL_MS), s, telemetry::stable::climb);
        terminate_buffer ();
    }

    STD_DRAW;

}  // namespace climb_rate

namespace flight_mode
{

    PANEL_NAME("FlightMode");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        osd::draw::rect(x, y, 6, 3);
        if (telemetry::status::flight_mode_name)
            max7456::puts(x + 1, y + 1, telemetry::status::flight_mode_name); // use RAM name
        else
            max7456::puts_p(x + 1, y + 1,
                    telemetry::status::flight_mode_name_p ? telemetry::status::flight_mode_name_p : PSTR(OSD_SYMBOL_NO_FLIGHT_MODE));
    //    max7456::open(x + 1, y + 1);
    //    fprintf_P(&max7456::stream, PSTR("%u"), telemetry::status::flight_mode);
    }

}  // namespace flight_mode

namespace armed_flag
{

    PANEL_NAME("ArmedFlag");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        uint8_t attr = telemetry::status::armed ? 0 : MAX7456_ATTR_INVERT;
        osd::draw::rect(x, y, 3, 3, true, attr);
        max7456::put(x + 1, y + 1, OSD_CHAR_ARMED_INV, attr);
    }

}  // namespace name

namespace connection_state
{

    PANEL_NAME("ConState");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        // TODO blink MAX7456_ATTR_INVERT -> MAX7456_ATTR_NONE when CONNECTION_STATE_ESTABLISHING
        uint8_t attr = telemetry::status::connection != telemetry::status::CONNECTED ? MAX7456_ATTR_INVERT : 0;

        osd::draw::rect(x, y, 3, 3, true, attr);
        max7456::put(x + 1, y + 1, OSD_CHAR_CONNECTION_INV, attr);
    }

}  // namespace connection_state

namespace flight_time
{

    STD_PANEL("FlightTime", 8, OSD_SYMBOL_TIME "%02u:%02u", telemetry::status::flight_time / 60, telemetry::status::flight_time % 60);

}  // namespace flight_time

namespace roll
{

    STD_PANEL("Roll", 7, OSD_SYMBOL_ROLL "%d" OSD_SYMBOL_DEGREES, (int16_t)telemetry::attitude::roll);

}  // namespace roll

namespace pitch
{

    STD_PANEL("Pitch", 7, OSD_SYMBOL_PITCH "%d" OSD_SYMBOL_DEGREES, (int16_t)telemetry::attitude::pitch);

}  // namespace pitch

namespace gps_state
{

    PANEL_NAME("GPS");

    DECLARE_BUF(4);

    STD_UPDATE("%d", telemetry::gps::satellites);

    void draw(uint8_t x, uint8_t y)
    {
        bool err = telemetry::gps::state == telemetry::GPS_STATE_NO_FIX;
        max7456::puts_p(x, y, PSTR(OSD_SYMBOL_SATELLITE), err ? MAX7456_ATTR_INVERT : 0);
        if (!err)
        {
            max7456::put(x + 2, y, telemetry::gps::state < telemetry::GPS_STATE_3D ? OSD_CHAR_2D : OSD_CHAR_3D,
                    telemetry::gps::state < telemetry::GPS_STATE_2D ? MAX7456_ATTR_BLINK : 0);
            if (telemetry::gps::satellites > 0)
                max7456::puts(x + 3, y, buffer);
        }
    }

}  // namespace gps_state

namespace gps_lat
{

    STD_PANEL("Lat", 11, OSD_SYMBOL_LAT "%02.6f", telemetry::gps::latitude);

}  // namespace gps_lat

namespace gps_lon
{

    STD_PANEL("Lon", 11, OSD_SYMBOL_LON "%02.6f", telemetry::gps::longitude);

}  // namespace gps_lon

namespace horizon
{

#define PANEL_HORIZON_WIDTH 14
#define PANEL_HORIZON_HEIGHT 5

#define _PAN_HORZ_CHAR_LINES 18
#define _PAN_HORZ_VRES 9
#define _PAN_HORZ_INT_WIDTH (PANEL_HORIZON_WIDTH - 2)
#define _PAN_HORZ_LINES (PANEL_HORIZON_HEIGHT * _PAN_HORZ_VRES)
#define _PAN_HORZ_TOTAL_LINES (PANEL_HORIZON_HEIGHT * _PAN_HORZ_CHAR_LINES)

    PANEL_NAME("Horizon");

	const char _line[PANEL_HORIZON_WIDTH + 1] PROGMEM   = OSD_SYMBOL_HORIZON_RULER_LEFT "            " OSD_SYMBOL_HORIZON_RULER_RIGHT;
	const char _center[PANEL_HORIZON_WIDTH + 1] PROGMEM = OSD_SYMBOL_HORIZON_RULER_LEFT_CENTER "            " OSD_SYMBOL_HORIZON_RULER_RIGHT_CENTER;
	char buffer[PANEL_HORIZON_HEIGHT][PANEL_HORIZON_WIDTH + 1];

    float __attribute__ ((noinline)) deg2rad(float deg)
    {
        return deg * 0.017453293;
    }

    void update()
    {
        for (uint8_t i = 0; i < PANEL_HORIZON_HEIGHT; i++)
        {
            memcpy_P(buffer[i], i == PANEL_HORIZON_HEIGHT / 2 ? _center : _line, PANEL_HORIZON_WIDTH);
            buffer[i][PANEL_HORIZON_WIDTH] = 0;
        }

        // code below from minoposd
        int16_t pitch_line = tan(deg2rad(telemetry::attitude::pitch)) * _PAN_HORZ_LINES;
        float roll = tan(deg2rad(telemetry::attitude::roll));
        for (uint8_t col = 1; col <= _PAN_HORZ_INT_WIDTH; col++)
        {
            // center X point at middle of each column
            float middle = col * _PAN_HORZ_INT_WIDTH - (_PAN_HORZ_INT_WIDTH * _PAN_HORZ_INT_WIDTH / 2) - _PAN_HORZ_INT_WIDTH / 2;
            // calculating hit point on Y plus offset to eliminate negative values
            int16_t hit = roll * middle + pitch_line + _PAN_HORZ_LINES;
            if (hit > 0 && hit < _PAN_HORZ_TOTAL_LINES)
            {
                int8_t row = PANEL_HORIZON_HEIGHT - ((hit - 1) / _PAN_HORZ_CHAR_LINES);
                int8_t subval = (hit - (_PAN_HORZ_TOTAL_LINES - row * _PAN_HORZ_CHAR_LINES + 1)) * _PAN_HORZ_VRES / _PAN_HORZ_CHAR_LINES;
                if (subval == _PAN_HORZ_VRES - 1)
                    buffer[row - 2][col] = OSD_CHAR_HORIZON_TOP;
                buffer[row - 1][col] = OSD_CHAR_HORIZON_LINE + subval;
            }
        }
    }

    void draw(uint8_t x, uint8_t y)
    {
        for (uint8_t i = 0; i < PANEL_HORIZON_HEIGHT; i++)
            max7456::puts(x, y + i, buffer[i]);
    }

}  // namespace horizon

namespace throttle
{

    STD_PANEL("Throttle", 7, OSD_SYMBOL_THROTTLE "%d%%", telemetry::input::throttle);

}  // namespace throttle

namespace groundspeed
{

    STD_PANEL("Groundspeed", 7, OSD_SYMBOL_GROUNDSPEED "%d" OSD_SYMBOL_KMH, (int16_t ) (telemetry::stable::groundspeed * 3.6));

}  // namespace ground_speed

namespace battery1_voltage
{

    PANEL_NAME("Bat1Voltage");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        osd::draw::battery_voltage(x, y, OSD_CHAR_BATTERY1, &telemetry::battery::battery1);
    }

}  // namespace battery_voltage

namespace battery2_voltage
{

    PANEL_NAME("Bat2Voltage");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        osd::draw::battery_voltage(x, y, OSD_CHAR_BATTERY2, &telemetry::battery::battery2);
    }

}  // namespace battery_voltage

namespace battery1_current
{

    STD_PANEL("Bat1Current", 8, OSD_SYMBOL_BAT1_CURRENT "%.2f" OSD_SYMBOL_A, telemetry::battery::battery1.amperage);

}  // namespace battery_current

namespace battery2_current
{

    STD_PANEL("Bat2Current", 8, OSD_SYMBOL_BAT2_CURRENT "%.2f" OSD_SYMBOL_A, telemetry::battery::battery2.amperage);

}  // namespace battery_current

namespace battery1_consumed
{

    STD_PANEL("Bat1Consumed", 8, OSD_SYMBOL_BAT1_CONSUMED "%u" OSD_SYMBOL_MAH, (uint16_t ) telemetry::battery::battery1.consumed);

}  // namespace battery_consumed

namespace battery2_consumed
{

    STD_PANEL("Bat2Consumed", 8, OSD_SYMBOL_BAT2_CONSUMED "%u" OSD_SYMBOL_MAH, (uint16_t ) telemetry::battery::battery2.consumed);

}  // namespace battery_consumed

namespace rssi_flag
{

    PANEL_NAME("RSSIFlag");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        if (telemetry::input::rssi_low)
            max7456::put(x, y, OSD_CHAR_RSSI_FLAG, MAX7456_ATTR_BLINK);
    }

}  // namespace rssi_flag

namespace home_distance
{

    PANEL_NAME("HomeDistance");

    DECLARE_BUF(8);
    uint8_t attr, i_attr;

    void update()
    {
        attr = telemetry::home::state == telemetry::home::NO_FIX ? MAX7456_ATTR_INVERT : 0;
        i_attr = telemetry::home::state != telemetry::home::FIXED ? MAX7456_ATTR_INVERT : 0;
        if (i_attr)
        {
            if (telemetry::home::state != telemetry::home::NO_FIX)
                snprintf_P(buffer, sizeof(buffer), PSTR(OSD_SYMBOL_NO_HOME_DISTANCE));
            return;
        }
        if (telemetry::home::distance >= 10000)
            snprintf_P(buffer, sizeof(buffer), PSTR("%.1f" OSD_SYMBOL_KM), telemetry::home::distance / 1000);
        else
            snprintf_P(buffer, sizeof(buffer), PSTR("%u" OSD_SYMBOL_M), (uint16_t) telemetry::home::distance);
    }

    void draw(uint8_t x, uint8_t y)
    {
        max7456::put(x, y, OSD_CHAR_HOME, i_attr);
        max7456::puts(x + 1, y, buffer, attr);
    }

}  // namespace home_distance

namespace home_direction
{

    PANEL_NAME("HomeDirection");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        if (telemetry::home::state == telemetry::home::FIXED)
            osd::draw::arrow(x, y, telemetry::home::direction);
    }

}  // namespace home_direction

namespace callsign
{

    PANEL_NAME("Callsign");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        max7456::puts(x, y, telemetry::status::callsign);
    }

}  // namespace callsign

namespace temperature
{

    STD_PANEL("Temperature", 9, OSD_SYMBOL_TEMPERATURE "%.1f" OSD_SYMBOL_CELSIUM, telemetry::environment::temperature);

}  // namespace temperature


namespace rssi
{

    PANEL_NAME("RSSI");

	const char _l0[] PROGMEM = OSD_SYMBOL_RSSI0;
	const char _l1[] PROGMEM = OSD_SYMBOL_RSSI1;
	const char _l2[] PROGMEM = OSD_SYMBOL_RSSI2;
	const char _l3[] PROGMEM = OSD_SYMBOL_RSSI3;
	const char _l4[] PROGMEM = OSD_SYMBOL_RSSI4;
	const char _l5[] PROGMEM = OSD_SYMBOL_RSSI5;

	const char * const levels[] PROGMEM = { _l0, _l1, _l2, _l3, _l4, _l5 };

	const char *scale = NULL;

    void update()
    {
        uint8_t level = (telemetry::input::rssi + 10) / 20;
        if (level == 0 && telemetry::input::rssi > 0)
            level = 1;
        if (level > 5)
            level = 5;
        scale = (const char *) pgm_read_ptr(&levels[level]);
    }

    void draw(uint8_t x, uint8_t y)
    {
        max7456::puts_p(x, y, scale);
    }

}  // namespace rssi

namespace compass
{

    PANEL_NAME("Compass");

	// Code from MinOpOSD
	const uint8_t ruler[] PROGMEM = {
	    OSD_CHAR_COMPASS_N, OSD_CHAR_COMPASS_LINE, OSD_CHAR_COMPASS_TICK, OSD_CHAR_COMPASS_LINE,
	    OSD_CHAR_COMPASS_E, OSD_CHAR_COMPASS_LINE, OSD_CHAR_COMPASS_TICK, OSD_CHAR_COMPASS_LINE,
	    OSD_CHAR_COMPASS_S, OSD_CHAR_COMPASS_LINE, OSD_CHAR_COMPASS_TICK, OSD_CHAR_COMPASS_LINE,
	    OSD_CHAR_COMPASS_W, OSD_CHAR_COMPASS_LINE, OSD_CHAR_COMPASS_TICK, OSD_CHAR_COMPASS_LINE,
	};

    const int8_t ruler_size = sizeof(ruler);

    DECLARE_BUF(12);

    uint8_t attr, arrow;

    void update()
    {
        int16_t offset = round(telemetry::stable::heading * ruler_size / 360.0) - (sizeof(buffer) - 1) / 2;
        if (offset < 0)
            offset += ruler_size;
        for (uint8_t i = 0; i < sizeof(buffer) - 1; i++)
        {
            buffer[i] = pgm_read_byte(&ruler[offset]);
            if (++offset >= ruler_size)
                offset = 0;
        }
        terminate_buffer();
        switch (telemetry::stable::heading_source)
        {
            case telemetry::stable::DISABLED:
                attr = MAX7456_ATTR_INVERT;
                arrow = OSD_CHAR_COMPASS_DEFAULT;
                break;
            case telemetry::stable::GPS:
                attr = 0;
                arrow = OSD_CHAR_COMPASS_GPS;
                break;
            case telemetry::stable::INTERNAL_MAG:
                attr = 0;
                arrow = OSD_CHAR_COMPASS_INTERNAL;
                break;
            case telemetry::stable::EXTERNAL_MAG:
                attr = 0;
                arrow = OSD_CHAR_COMPASS_EXTERNAL;
                break;
            default:
                attr = 0;
                arrow = OSD_CHAR_COMPASS_DEFAULT;
        }
    }

    void draw(uint8_t x, uint8_t y)
    {
        max7456::put(x + (sizeof(buffer) - 1) / 2, y, arrow, attr);
        max7456::puts(x, y + 1, buffer, attr);
    }

}  // namespace compass

namespace airspeed
{

    STD_PANEL("Airspeed", 7, OSD_SYMBOL_AIRSPEED "%d" OSD_SYMBOL_KMH, (int16_t ) (telemetry::stable::airspeed * 3.6));

}  // namespace airspeed

namespace crosshair
{

    PANEL_NAME("Crosshair");

    void update() {}

    void draw(uint8_t x, uint8_t y)
    {
        max7456::puts_p(x, y, PSTR(OSD_SYMBOL_CROSSHAIR), 0);
    }

}  // namespace crosshair

}  // namespace panels

namespace panel
{

#define declare_panel(NS) { osd::panels:: NS ::name, osd::panels:: NS ::update, osd::panels:: NS ::draw }

const panel_t panels[] PROGMEM = {
    declare_panel(alt),
    declare_panel(climb),
    declare_panel(flight_mode),
    declare_panel(armed_flag),
    declare_panel(connection_state),
    declare_panel(flight_time),
    declare_panel(roll),
    declare_panel(pitch),
    declare_panel(gps_state),
    declare_panel(gps_lat),
    declare_panel(gps_lon),
    declare_panel(horizon),
    declare_panel(throttle),
    declare_panel(groundspeed),
    declare_panel(battery1_voltage),
    declare_panel(battery2_voltage),
    declare_panel(battery1_current),
    declare_panel(battery2_current),
    declare_panel(battery1_consumed),
    declare_panel(battery2_consumed),
    declare_panel(rssi_flag),
    declare_panel(home_distance),
    declare_panel(home_direction),
    declare_panel(callsign),
    declare_panel(temperature),
    declare_panel(rssi),
    declare_panel(compass),
    declare_panel(airspeed),
    declare_panel(crosshair)
};

const uint8_t count = sizeof(panels) / sizeof(panel_t);

}  // namespace panels

}  // namespace osd
