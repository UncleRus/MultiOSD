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

#define INPUT_CHANNELS 9

namespace telemetry
{

namespace status
{
	extern uint8_t connection;		// CONNECTION_STATE_xxx enum
	extern uint16_t flight_time;	// seconds
	extern uint8_t flight_mode;		// FLIGHT_MODE_xxx enum
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
	extern int16_t throttle;		// percents
	extern int16_t roll;			// percents
	extern int16_t pitch;			// percents
	extern int16_t yaw;				// percents
	extern int16_t collective;		// percents
	extern int16_t thrust;			// percents
	extern int16_t channels [INPUT_CHANNELS]; // raw values
}

namespace gps
{
	extern float latitude;
	extern float longitude;
	extern float altitude;			// meters
	extern float speed;				// m/s
	extern float heading;
	extern int8_t sattelites;
	extern uint8_t state;			// GPS_STATE_xxx enum
	extern float climb;				// m/s
}

namespace barometer
{
	extern float altitude;			// meters
}

namespace stable
{
	extern float climb;				// m/s
	extern float altitude;			// meters
	extern float ground_speed;		// m/s
	extern float air_speed;			// m/s
}

namespace battery
{
	extern float voltage;			// volts
	extern float current;			// amperes
	extern uint16_t consumed;		// mAh
}

namespace messages
{
	extern bool battery_low;
	extern bool rssi_low;
}

// init non-uavtalk telemetry modules
void init ();
// update from non-uavtalk telemetry modules
bool update ();

namespace settings
{
	void reset ();
}

}

#endif /* TELEMETRY_TELEMETRY_H_ */
