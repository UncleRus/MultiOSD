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
#ifndef TELEMETRY_TELEMETRY_H_
#define TELEMETRY_TELEMETRY_H_

#include <stdint.h>
#include <stdio.h>
#include "../lib/pgmspace.h"
#include "../config.h"

#define INPUT_CHANNELS 10

namespace telemetry
{

enum gps_state_t
{
	GPS_STATE_NO_FIX = 0,
	GPS_STATE_FIXING = 1,
	GPS_STATE_2D     = 2,
	GPS_STATE_3D     = 3
};

namespace settings
{

	void init ();
	void reset ();

}  // namespace settings

extern uint32_t update_time;             // update time

namespace status
{
	enum connection_state_t
	{
		DISCONNECTED = 0,
		ESTABLISHING = 1,
		CONNECTED    = 2
	};

	extern const char *       callsign;                        // 8 chars max
	extern connection_state_t connection;
	extern uint16_t           flight_time;                     // seconds
	extern uint8_t            flight_mode;
	extern const char *       flight_mode_name_p;              // progmem string
	extern const char *       flight_mode_name;                // SRAM string
	extern bool               armed;
}

namespace attitude
{
	extern float roll;   // degrees
	extern float pitch;  // degrees
	extern float yaw;    // degrees
}

namespace input
{
	extern bool     connected;                 // input values are valid
	extern uint8_t  rssi;                      // percents
	extern bool     rssi_low;
	extern int8_t   throttle;                  // percents
	extern int8_t   roll;                      // percents
	extern int8_t   pitch;                     // percents
	extern int8_t   yaw;                       // percents
	extern uint16_t channels [INPUT_CHANNELS]; // raw values

	void set_rssi (uint8_t value);
}

namespace gps
{
	extern float       latitude;   // degrees
	extern float       longitude;  // degrees
	extern float       altitude;   // meters
	extern float       speed;      // m/s
	extern uint16_t    heading;    // degrees, 0..360
	extern int8_t      satellites;
	extern gps_state_t state;      // GPS_STATE_xxx enum
	extern float       climb;      // m/s

	extern float hdop;
	extern float vdop;
	extern float pdop;
}

namespace barometer
{
	extern float    altitude;    // meters
	extern float    temperature; // Celsius
	extern uint32_t pressure;    // Pa
}

namespace environment
{
	extern uint16_t wind_direction; // degress
	extern float    wind_speed;     // m/s in ground plane
	extern float    temperature;    // Celsius
}

namespace stable
{
	enum heading_source_t
	{
		DISABLED = 0,
		GPS = 1,
		INTERNAL_MAG = 2,
		EXTERNAL_MAG = 3
	};

	extern float    climb;        // m/s
	extern float    altitude;     // meters
	extern float    groundspeed;  // m/s
	extern float    airspeed;     // m/s
	extern uint16_t heading;      // degrees, 0..360
	extern heading_source_t heading_source;

	void update_alt_climb (float altitude);
	void calc_heading (float x, float y);
}

namespace battery
{
	struct battery_t
	{
		float   voltage;      // Volts
		uint8_t cells;
		float   cell_voltage; // Volts
		uint8_t level;        // percents
		bool    low;
		float   amperage;      // Amperes
		float   consumed;      // mAh

		battery_t ()
			: voltage (0), cells (0), cell_voltage (0), level (0), low (true), amperage (0), consumed (0)
		{}

		void set_voltage (float value, bool calc_cells);
		void set_amperage (float value, uint16_t interval);
	};

	extern battery_t battery1;
	extern battery_t battery2;
}

namespace home
{
	enum home_state_t
	{
		NO_FIX = 0,
		FIXING = 1,
		FIXED  = 2
	};

	extern home_state_t state;
	extern float        distance;  // meters
	extern uint16_t     direction; // degrees, 0..360

	extern float        longitude; // degrees
	extern float        latitude;  // degrees
	extern float        altitude;  // meters

	// try to fix home position
	void fix ();

	// read gps and recalc
	void update ();
}

namespace waypoint
{

	extern uint8_t  current;  // current waypoint number
	extern uint8_t  prev;     // previous waypoint
	extern uint16_t distance; // meters, distance to active waypoint
	extern uint16_t bearing;  // degrees, 0..360

	void set_bearing (int16_t value);
	// set current waypoint
	void set (uint8_t value);
}

void init ();
bool update ();

typedef bool (* parser_t) (uint8_t b);
bool receive (parser_t parser);

namespace modules
{

	struct module_t
	{
		typedef void (* proc_t) ();
		typedef bool (* update_t) ();

		const char *name_p;
		proc_t init_settings;
		proc_t reset_settings;
		proc_t init;
		update_t update;
	};

	// telemetry modules collection
	extern const module_t modules [] PROGMEM;
	extern const uint8_t count;

	inline const char *name_p (uint8_t module)
	{
		return (const char *) pgm_read_ptr (&modules [module].name_p);
	}

	inline void init_settings (uint8_t module)
	{
		((module_t::proc_t) pgm_read_ptr (&modules [module].init_settings)) ();
	}

	inline void reset_settings (uint8_t module)
	{
		((module_t::proc_t) pgm_read_ptr (&modules [module].reset_settings)) ();
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

}  // namespace telemetry

#endif /* TELEMETRY_TELEMETRY_H_ */
