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
#include "telemetry.h"
#include "adc_battery/adc_battery.h"
#include "../../config.h"

#ifdef TELEMETRY_MODULES_ADC_BATTERY
#	include "adc_battery/adc_battery.h"
#endif

#ifdef TELEMETRY_MODULES_UAVTALK
#	include "uavtalk/uavtalk.h"
#endif

namespace telemetry
{

namespace status
{
	uint8_t connection = CONNECTION_STATE_DISCONNECTED;
	uint16_t flight_time = 0;
	uint8_t flight_mode = FLIGHT_MODE_MANUAL;
	bool armed = false;
	uint8_t rssi = 100;
}

namespace attitude
{
	float roll = 0;
	float pitch = 0;
	float yaw = 0;
}

namespace input
{
	int16_t throttle = 0;
	int16_t roll = 0;
	int16_t pitch = 0;
	int16_t yaw = 0;
	int16_t collective = 0;
	int16_t thrust = 0;
	int16_t channels [INPUT_CHANNELS];
}

namespace gps
{
	float latitude = 0.0;
	float longitude = 0.0;
	float altitude = 0.0;
	float speed = 0.0;
	float heading = 0.0;
	int8_t sattelites = 0;
	uint8_t state = GPS_STATE_NO_FIX;
	float climb = 0.0;
}

namespace barometer
{
	float altitude = 0.0;
}

namespace stable
{
	float climb = 0.0;
	float altitude = 0.0;
	float ground_speed = 0.0;		// m/s
	float air_speed = 0.0;			// m/s
}

namespace battery
{
	float voltage = 0.0;
	float current = 0.0;
	uint16_t consumed = 0;
}

namespace messages
{
	bool battery_low = false;
	bool rssi_low = false;
}


void init ()
{
	// FIXME: Use eeprom instead of macros
#ifdef TELEMETRY_MODULES_UAVTALK
	uavtalk::init ();
#endif
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	adc_battery::init ();
#endif
}

bool update ()
{
	bool res = false;
#ifdef TELEMETRY_MODULES_UAVTALK
	res |= uavtalk::update ();
#endif
#ifdef TELEMETRY_MODULES_ADC_BATTERY
	res |= adc_battery::update ();
#endif
#ifdef TELEMETRY_MODULES_BARO
	// TODO: calc stable alt/climb/speed values based on gps/baro/sensors
	if ()
#endif
	return res;
}

}
