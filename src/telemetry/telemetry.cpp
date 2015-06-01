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
#include "../config.h"
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <math.h>
#include <string.h>
#include "../lib/timer/timer.h"
#include "../settings.h"

namespace telemetry
{

namespace status
{

	char callsign [5] = "NONE";
	uint8_t connection = CONNECTION_STATE_DISCONNECTED;
	uint16_t flight_time = 0;
	uint8_t flight_mode = FLIGHT_MODE_MANUAL;
	const char *flight_mode_name_p = NULL;
	bool armed = false;

}  // namespace status

namespace attitude
{

	float roll = 0;
	float pitch = 0;
	float yaw = 0;

}  // namespace attitude

namespace input
{

	uint8_t rssi = 0;
	bool connected = false;
	int16_t throttle = 0;
	int16_t roll = 0;
	int16_t pitch = 0;
	int16_t yaw = 0;
	int16_t collective = 0;
	int16_t thrust = 0;
	uint8_t flight_mode_switch = 0;
	uint16_t channels [INPUT_CHANNELS];

}  // namespace input

namespace gps
{

	float latitude = 0.0;
	float longitude = 0.0;
	float altitude = 0.0;
	float speed = 0.0;
	uint16_t heading = 0;
	int8_t satellites = 0;
	uint8_t state = GPS_STATE_NO_FIX;
	float climb = 0.0;

}  // namespace gps

namespace barometer
{

	int16_t altitude = 0;
	float temperature = 0.0;
	int16_t pressure = 0;

}  // namespace barometer

namespace stable
{

	float climb = 0.0;
	float altitude = 0.0;
	float ground_speed = 0.0;
	float air_speed = 0.0;
	int16_t temperature = 0;
	uint16_t heading = 0;

	static uint32_t _alt_update_time = 0;

	void update_alt_climb (float _alt)
	{
		uint32_t ticks = timer::ticks ();
		climb = (_alt - altitude) / (ticks - _alt_update_time) * 1000;
		altitude = _alt;
		_alt_update_time = ticks;
	}

}  // namespace stable

#define TELEMETRY_EEPROM_MAIN_MODULE_ID		_eeprom_byte (TELEMETRY_EEPROM_OFFSET)
#define TELEMETRY_EEPROM_MIN_CELL_VOLTAGE	_eeprom_float (TELEMETRY_EEPROM_OFFSET + 1)
#define TELEMETRY_EEPROM_NOM_CELL_VOLTAGE	_eeprom_float (TELEMETRY_EEPROM_OFFSET + 5)
#define TELEMETRY_EEPROM_MAX_CELL_VOLTAGE	_eeprom_float (TELEMETRY_EEPROM_OFFSET + 9)
#define TELEMETRY_EEPROM_LOW_VOLTAGE		_eeprom_float (TELEMETRY_EEPROM_OFFSET + 13)
#define TELEMETRY_EEPROM_CALLSIGN			_eeprom_dword (TELEMETRY_EEPROM_OFFSET + 17)

namespace battery
{

	float min_cell_voltage = BATTERY_MIN_CELL_VOLTAGE;
	float nom_cell_voltage = BATTERY_NOM_CELL_VOLTAGE;
	float max_cell_voltage = BATTERY_MAX_CELL_VOLTAGE;
	float low_cell_voltage = BATTERY_LOW_CELL_VOLTAGE;

	float voltage = 0;
	float current = 0;
	float consumed = 0;
	uint8_t cells = 0;
	float cell_voltage = 0;
	uint8_t level = 0;

	static float _cell_range;

	void reset ()
	{
		eeprom_update_float (TELEMETRY_EEPROM_MIN_CELL_VOLTAGE, BATTERY_MIN_CELL_VOLTAGE);
		eeprom_update_float (TELEMETRY_EEPROM_NOM_CELL_VOLTAGE, BATTERY_NOM_CELL_VOLTAGE);
		eeprom_update_float (TELEMETRY_EEPROM_MAX_CELL_VOLTAGE, BATTERY_MAX_CELL_VOLTAGE);
		eeprom_update_float (TELEMETRY_EEPROM_LOW_VOLTAGE, BATTERY_LOW_CELL_VOLTAGE);
	}

	void init ()
	{
		min_cell_voltage = eeprom_read_float (TELEMETRY_EEPROM_MIN_CELL_VOLTAGE);
		nom_cell_voltage = eeprom_read_float (TELEMETRY_EEPROM_NOM_CELL_VOLTAGE);
		max_cell_voltage = eeprom_read_float (TELEMETRY_EEPROM_MAX_CELL_VOLTAGE);
		low_cell_voltage = eeprom_read_float (TELEMETRY_EEPROM_LOW_VOLTAGE);
		_cell_range = max_cell_voltage - min_cell_voltage;
	}

	void update_voltage ()
	{
		if (!cells) cells = round (voltage / nom_cell_voltage);
		//cells = (uint8_t)(voltage / max_cell_voltage) + 1;
		if (cells)
		{
			cell_voltage = voltage / cells;
			messages::battery_low = cell_voltage <= low_cell_voltage;
		}
		level = cell_voltage > min_cell_voltage
			? (cell_voltage - min_cell_voltage) / _cell_range * 100
			: 0;
		if (level > 100) level = 100;
	}

}  // namespace battery

namespace messages
{

	bool battery_low = false;
	bool rssi_low = false;

}  // namespace messages

namespace home
{

	uint8_t state = HOME_STATE_NO_FIX;

	float distance = 0;
	uint8_t direction = HOME_DIR_N;

	float longitude;
	float latitude;
	float altitude = 0;

//	static uint16_t _fix_timeout = 3000;
//	static uint32_t _fix_timestop = 0;

	void fix ()
	{
		state = HOME_STATE_FIXING;
	}

	void update ()
	{
		//uint32_t ticks = timer::ticks ();
		if (state == HOME_STATE_NO_FIX) return;
		if (state == HOME_STATE_FIXING)
			switch (gps::state)
			{
				case GPS_STATE_NO_FIX:
					state = HOME_STATE_NO_FIX;
					break;
				case GPS_STATE_FIXING:
				case GPS_STATE_2D:
					if (state == HOME_STATE_NO_FIX) state = HOME_STATE_FIXING;
					break;
				case GPS_STATE_3D:
					state = HOME_STATE_FIXED;
					longitude = gps::longitude;
					latitude = gps::latitude;
					altitude = stable::altitude;
					break;
			}
		if (state != HOME_STATE_FIXED) return;

		float rads = fabs (latitude) * 0.0174532925;
		double scale_down = cos (rads);
		double scale_up = 1.0 / cos (rads);

		// distance
		float dstlat = fabs (latitude - gps::latitude) * 111319.5;
		float dstlon = fabs (longitude - gps::longitude) * 111319.5 * scale_down;
		distance = sqrt (square (dstlat) + square (dstlon));

		// DIR to Home
		dstlon = (longitude - gps::longitude); 				// x offset
		dstlat = (latitude - gps::latitude) * scale_up; 	// y offset
		int16_t bearing = 90 + atan2 (dstlat, -dstlon) * 57.295775; // absolute home direction
		if (bearing < 0) bearing += 360;	// normalization
		bearing -= 180;						// absolute return direction
		if (bearing < 0) bearing += 360;	// normalization
		bearing -= gps::heading;			// relative home direction
		if (bearing < 0) bearing += 360;	// normalization
		direction = round ((float) (bearing / 360.0f) * 16.0f) + 1;
		if (direction > 15) direction = 0;
	}

}  // namespace home

}  // namespace telemetry

#ifdef TELEMETRY_MODULES_ADC_BATTERY
#	include "adc_battery/adc_battery.h"
#endif
#ifdef TELEMETRY_MODULES_I2C_BARO
#	include "i2c_baro/i2c_baro.h"
#endif
#ifdef TELEMETRY_MODULES_I2C_COMPASS
#	include "i2c_compass/i2c_compass.h"
#endif
#ifdef TELEMETRY_MODULES_UAVTALK
#	include "uavtalk/uavtalk.h"
#endif
#ifdef TELEMETRY_MODULES_MAVLINK
#	include "mavlink/mavlink.h"
#endif

#ifndef TELEMETRY_MAIN_MODULE_ID
#	error No main telemetry module defined
#endif

#define _declare_module(NS) { telemetry::modules:: NS ::__name, telemetry::modules:: NS ::reset, \
	telemetry::modules:: NS ::init, telemetry::modules:: NS ::update }

namespace telemetry
{

namespace modules
{

	const module_t modules [] PROGMEM = {
#ifdef TELEMETRY_MODULES_ADC_BATTERY
		_declare_module (adc_battery),
#endif
#ifdef TELEMETRY_MODULES_I2C_BARO
		_declare_module (i2c_baro),
#endif
#ifdef TELEMETRY_MODULES_I2C_COMPASS
		_declare_module (i2c_compass),
#endif
#ifdef TELEMETRY_MODULES_UAVTALK
		_declare_module (uavtalk),
#endif
#ifdef TELEMETRY_MODULES_MAVLINK
		_declare_module (mavlink),
#endif
	};

	const uint8_t count = sizeof (modules) / sizeof (module_t);

}  // namespace modules


void init ()
{
	if (eeprom_read_byte (TELEMETRY_EEPROM_MAIN_MODULE_ID) != TELEMETRY_MAIN_MODULE_ID)
		::settings::reset ();

	uint32_t cs = eeprom_read_dword (TELEMETRY_EEPROM_CALLSIGN);
	memcpy (status::callsign, &cs, 4);
	status::callsign [4] = 0;

	battery::init ();
	for (uint8_t i = 0; i < modules::count; i ++)
		modules::init (i);
}

bool update ()
{
	bool res = false;
	for (uint8_t i = 0; i < modules::count; i ++)
		res |= modules::update (i);
	return res;
}

namespace settings
{
	void reset ()
	{
		eeprom_update_byte (TELEMETRY_EEPROM_MAIN_MODULE_ID, TELEMETRY_MAIN_MODULE_ID);
		eeprom_update_dword (TELEMETRY_EEPROM_CALLSIGN, DEFAULT_CALLSIGN);
		battery::reset ();
		for (uint8_t i = 0; i < modules::count; i ++)
			modules::reset (i);
	}

}  // namespace settings


}  // namespace telemetry
