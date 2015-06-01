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
#ifndef TELEMETRY_TELEMETRY_H_
#define TELEMETRY_TELEMETRY_H_

#include <stdint.h>
#include <stdio.h>
#include <avr/pgmspace.h>

#define GPS_STATE_NO_FIX	0
#define GPS_STATE_FIXING	1
#define GPS_STATE_2D		2
#define GPS_STATE_3D		3

#define FLIGHT_MODE_MANUAL			0
#define FLIGHT_MODE_STABILIZED1		1
#define FLIGHT_MODE_STABILIZED2		2
#define FLIGHT_MODE_STABILIZED3		3
#define FLIGHT_MODE_STABILIZED4		4
#define FLIGHT_MODE_STABILIZED5		5
#define FLIGHT_MODE_STABILIZED6		6
#define FLIGHT_MODE_AUTOTUNE		7
#define FLIGHT_MODE_POS_HOLD		8
#define FLIGHT_MODE_POS_VARIO_FPV	9
#define FLIGHT_MODE_POS_VARIO_LOS	10
#define FLIGHT_MODE_POS_VARIO_NSEW	11
#define FLIGHT_MODE_RETURNTOBASE	12
#define FLIGHT_MODE_LAND			13
#define FLIGHT_MODE_PATHPLANNER		14
#define FLIGHT_MODE_POI				15
#define FLIGHT_MODE_AUTOCRUISE		16

#define CONNECTION_STATE_DISCONNECTED	0
#define CONNECTION_STATE_ESTABLISHING	1
#define CONNECTION_STATE_CONNECTED		2

#define HOME_STATE_NO_FIX	0
#define HOME_STATE_FIXING	1
#define HOME_STATE_FIXED	2

/*
 * Direction to home. 16 rhumbs clockwise: 0 - forward, 4 - right, 8 - backward, 12 - left
 */
#define HOME_DIR_N   0	// 12 hours
#define HOME_DIR_NNE 1
#define HOME_DIR_NE  2
#define HOME_DIR_ENE 3
#define HOME_DIR_E   4	// 3 hours
#define HOME_DIR_ESE 5
#define HOME_DIR_SE  6
#define HOME_DIR_SSE 7
#define HOME_DIR_S   8	// 6 hours
#define HOME_DIR_SSW 9
#define HOME_DIR_SW  10
#define HOME_DIR_WSW 11
#define HOME_DIR_W   12	// 9 hours
#define HOME_DIR_WNW 13
#define HOME_DIR_NW  14
#define HOME_DIR_NNW 15

#define INPUT_CHANNELS 9

namespace telemetry
{

namespace status
{
	extern char callsign [];	// 4 chars max
	extern uint8_t connection;		// CONNECTION_STATE_xxx enum
	extern uint16_t flight_time;	// seconds
	extern uint8_t flight_mode;		//
	extern const char *flight_mode_name_p; // progmem string
	extern bool armed;
	extern uint8_t rssi;			// percents
}

namespace attitude
{
	extern float roll;				// degrees
	extern float pitch;				// degrees
	extern float yaw;				// degrees
}

namespace input
{
	extern bool connected;			// input values are valid
	extern int16_t throttle;		// percents
	extern int16_t roll;			// percents
	extern int16_t pitch;			// percents
	extern int16_t yaw;				// percents
	extern int16_t collective;		// percents
	extern int16_t thrust;			// percents
	extern uint8_t flight_mode_switch;	// switch position
	extern uint16_t channels [INPUT_CHANNELS]; // raw values
}

namespace gps
{
	extern float latitude;
	extern float longitude;
	extern float altitude;			// meters
	extern float speed;				// m/s
	extern uint16_t heading;		// degrees, 0..360
	extern int8_t satellites;
	extern uint8_t state;			// GPS_STATE_xxx enum
	extern float climb;				// m/s
}

namespace barometer
{
	extern int16_t altitude;		// meters
	extern float temperature;		// Celsius
	extern int16_t pressure;		// Pascal
}

namespace stable
{
	extern float climb;				// m/s
	extern float altitude;			// meters
	extern float ground_speed;		// m/s
	extern float air_speed;			// m/s
	extern int16_t temperature;		// Celsius
	extern uint16_t heading;		// degrees, 0..360

	void update_alt_climb (float altitude);
}

namespace battery
{
	extern float max_cell_voltage;	// volts
	extern float nom_cell_voltage;	// volts
	extern float min_cell_voltage;	// volts
	extern float low_cell_voltage;	// volts

	extern float voltage;			// volts
	extern float current;			// amperes
	extern float consumed;			// mAh
	extern uint8_t cells;
	extern float cell_voltage;		// volts
	extern uint8_t level;			// percents

	void update_voltage ();
}

namespace messages
{
	extern bool battery_low;
	extern bool rssi_low;
}

namespace home
{
	extern uint8_t state;			// HOME_STATE_xxx enum
	extern float distance;			// meters
	extern uint8_t direction;		// HOME_DIR_xxx enum

	extern float longitude;
	extern float latitude;
	extern float altitude;			// meters

	void fix ();					// try to fix home
	void update (); 				// read gps and recalc
}

void init ();
bool update ();

namespace modules
{

	struct module_t
	{
		typedef void (* proc_t) ();
		typedef bool (* update_t) ();

		const char *name_p;
		proc_t reset;
		proc_t init;
		update_t update;
	};

	extern const module_t modules [] PROGMEM;
	extern const uint8_t count;

	inline const char *name_p (uint8_t module)
	{
		return (const char *) pgm_read_ptr (&modules [module].name_p);
	}

	inline void reset (uint8_t module)
	{
		((module_t::proc_t) pgm_read_ptr (&modules [module].reset)) ();
	}

	inline void init (uint8_t module)
	{
		((module_t::proc_t) pgm_read_ptr (&modules [module].init)) ();
	}

	inline bool update (uint8_t module)
	{
		return ((module_t::update_t) pgm_read_ptr (&modules [module].update)) ();
	}

}

namespace settings
{

	void reset ();

}  // namespace settings

}  // namespace telemetry

#endif /* TELEMETRY_TELEMETRY_H_ */
