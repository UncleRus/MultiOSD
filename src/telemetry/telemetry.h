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

#define CONNECTION_STATE_DISCONNECTED	0
#define CONNECTION_STATE_ESTABLISHING	1
#define CONNECTION_STATE_CONNECTED		2

#define HOME_STATE_NO_FIX	0
#define HOME_STATE_FIXING	1
#define HOME_STATE_FIXED	2

#define INPUT_CHANNELS 9

namespace telemetry
{

extern uint32_t ticks;		// update time

namespace status
{
	extern char callsign [];	// 4 chars max
	extern uint8_t connection;		// CONNECTION_STATE_xxx enum
	extern uint16_t flight_time;	// seconds
	extern uint8_t flight_mode;		//
	extern const char *flight_mode_name_p; // progmem string
	extern bool armed;
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
	extern uint8_t rssi;			// percents
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
	extern uint32_t pressure;		// Pa
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
	void update_consumed ();
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
	extern uint16_t direction;		// degrees, 0..360

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
