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
#include "msp.h"
#include "multiwii.h"
#include "../telemetry.h"
#include "../../settings.h"
#include "../../eeprom.h"
#include "../../lib/uart/uart.h"
#include "../../lib/timer/timer.h"
#include <string.h>
#include <math.h>

#ifdef DEBUG
	#include "../../lib/dbgconsole.h"
#endif

namespace telemetry
{

namespace modules
{

namespace msp
{

namespace settings
{

#define EEPROM_ADDR_INTERNAL_HOME_CALC _eeprom_byte (UAVTALK_EEPROM_OFFSET)
#define EEPROM_ADDR_AMPERAGE_DIVIDER   _eeprom_word (UAVTALK_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_GPS_ALTITUDE       _eeprom_byte (UAVTALK_EEPROM_OFFSET + 3)
#define EEPROM_ADDR_BITRATE            _eeprom_byte (UAVTALK_EEPROM_OFFSET + 4)

const char __opt_mspihc [] PROGMEM = "MSPIHC";
const char __opt_mspad  [] PROGMEM = "MSPAD";
const char __opt_mspga  [] PROGMEM = "MSPGA";
const char __opt_mspbr  [] PROGMEM = "MSPBR";

const ::settings::option_t __settings [] PROGMEM = {
	declare_bool_option   (__opt_mspihc, EEPROM_ADDR_INTERNAL_HOME_CALC),
	declare_uint16_option (__opt_mspad,  EEPROM_ADDR_AMPERAGE_DIVIDER),
	declare_bool_option   (__opt_mspga,  EEPROM_ADDR_GPS_ALTITUDE),
	declare_uint8_option  (__opt_mspbr,  EEPROM_ADDR_BITRATE),
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_INTERNAL_HOME_CALC, MSP_DEFAULT_INTERNAL_HOME_CALC);
	eeprom_update_word (EEPROM_ADDR_AMPERAGE_DIVIDER, MSP_DEFAULT_AMPERAGE_DIVIDER);
	eeprom_update_byte (EEPROM_ADDR_GPS_ALTITUDE, MSP_DEFAULT_GPS_ALTITUDE);
	eeprom_update_byte (EEPROM_ADDR_BITRATE, MSP_DEFAULT_BITRATE);
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////
// parser
///////////////////////////////////////////////////////////////////////////////

enum parser_state_t
{
	PS_SYNC1,
	PS_SYNC2,
	PS_DIR,
	PS_SIZE,
	PS_CMD,
	PS_DATA,
	PS_CRC
};

struct message_t
{
	union payload_t
	{
		uint8_t data [1];
		multiwii::ident_t ident;
		multiwii::rc_t rc;
		multiwii::raw_gps_t raw_gps;
		multiwii::comp_gps_t comp_gps;
		multiwii::status_t status;
		multiwii::attitude_t attitude;
		multiwii::analog_t analog;
		multiwii::altitude_t altitude;
		multiwii::raw_imu_t raw_imu;
	};

	uint8_t pos;
	uint8_t cmd;
	uint8_t size;
	uint8_t crc;
	payload_t payload;

	void set_size (uint8_t b)
	{
		pos = 0;
		size = b;
		crc = b;
	}

	void set_cmd (uint8_t b)
	{
		cmd = b;
		crc ^= b;
	}

	bool append (uint8_t b)
	{
		payload.data [pos ++] = b;
		crc ^= b;
		return pos == size - 1;
	}
};

parser_state_t state;
message_t buffer;

bool parse (uint8_t b)
{
	switch (state)
	{
		case PS_SYNC1:
			state = b == MULTIWII_SYNC1 ? PS_SYNC2 : PS_SYNC1;
			break;
		case PS_SYNC2:
			state = b == MULTIWII_SYNC2 ? PS_DIR : PS_SYNC1;
			break;
		case PS_DIR:
			state = b == MULTIWII_OUT_DIR ? PS_SIZE : PS_SYNC1;
			break;
		case PS_SIZE:
			if (b <= sizeof (message_t::payload_t))
			{
				buffer.set_size (b);
				state = PS_CMD;
			}
			else state = PS_SYNC1;
			break;
		case PS_CMD:
			buffer.set_cmd (b);
			state = buffer.size ? PS_DATA : PS_CRC;
			break;
		case PS_DATA:
			if (buffer.append (b)) state = PS_CRC;
			break;
		case PS_CRC:
			state = PS_SYNC1;
			return buffer.crc == b;
	}
	return false;
}

///////////////////////////////////////////////////////////////////////////////
// requests
///////////////////////////////////////////////////////////////////////////////

namespace requests
{

struct period_t
{
	uint8_t cmd;
	uint16_t period; // ms
	uint16_t timeout; // ms
	uint16_t sensors_mask;
};

struct request_t
{
	uint8_t cmd;
	bool sent;
	uint16_t period; // ms
	uint16_t timeout;
	uint32_t next_update;

	void init (uint8_t cmd_, uint16_t period_, uint16_t timeout_)
	{
		sent = false;
		cmd = cmd_;
		period = period_;
		timeout = timeout_;
		next_update = 0;
	}

	bool run ()
	{
		if (!period || (sent && !timeout)) return false;

		uint32_t t = timer::ticks ();
		if (t < next_update || (sent && t - next_update < timeout)) return false;

		sent = true;
		next_update = t + period;

		TELEMETRY_UART::send (MULTIWII_SYNC1);
		TELEMETRY_UART::send (MULTIWII_SYNC2);
		TELEMETRY_UART::send (MULTIWII_IN_DIR);
		TELEMETRY_UART::send (0);   // size
		TELEMETRY_UART::send (cmd); // cmd
		TELEMETRY_UART::send (cmd); // crc = 0 ^ 0 ^ cmd

		return true;
	}

	void receive ()
	{
		sent = false;
	}
};

// request rates
const period_t periods [] PROGMEM = {
	{multiwii::MSP_STATUS,   500, 1000, 0},                               // 10 Hz
//	{multiwii::MSP_RC,       250, 0,    0},                               // 4 Hz
//	{multiwii::MSP_RAW_GPS,  200, 0,    _BV (multiwii::SENSOR_BIT_GPS)},  // 5 Hz
//	{multiwii::MSP_COMP_GPS, 200, 0,    _BV (multiwii::SENSOR_BIT_GPS)},  // 5 Hz
	{multiwii::MSP_ATTITUDE, 100, 1000, 0},                               // 20 Hz
//	{multiwii::MSP_ANALOG,   250, 0,    0},                               // 4 Hz
//	{multiwii::MSP_ALTITUDE, 100, 0,    _BV (multiwii::SENSOR_BIT_BARO)}, // 10 Hz
//	{multiwii::MSP_RAW_IMU,  250, 0,    _BV (multiwii::SENSOR_BIT_MAG)},  // 4 Hz, mag data only
};
const uint8_t requests_len = sizeof (periods) / sizeof (period_t);

request_t requests [requests_len];

void init ()
{
	for (uint8_t i = 0; i < requests_len; i ++)
		requests [i].init (pgm_read_byte (&periods [i].cmd), pgm_read_word (&periods [i].period), pgm_read_word (&periods [i].timeout));
}

void update (uint16_t msp_sensors)
{
	for (uint8_t i = 0; i < requests_len; i ++)
		if (periods [i].sensors_mask)
			requests [i].period = (msp_sensors & periods [i].sensors_mask) == periods [i].sensors_mask
				? periods [i].period
				: 0;
}

void run ()
{
	for (uint8_t i = 0; i < requests_len; i ++)
		if (requests [i].run ())
			break;
}

void receive (uint8_t cmd)
{
	for (uint8_t i = 0; i < requests_len; i ++)
		if (requests [i].cmd == cmd)
			requests [i].receive ();
}

}  // namespace requests

///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////

bool internal_home_calc;
uint16_t amperage_divider;
bool gps_altitude;
bool mag_enabled = false;
uint32_t connection_timeout;

void init ()
{
	internal_home_calc = eeprom_read_byte (EEPROM_ADDR_INTERNAL_HOME_CALC);
	amperage_divider = eeprom_read_word (EEPROM_ADDR_AMPERAGE_DIVIDER);
	gps_altitude = eeprom_read_byte (EEPROM_ADDR_GPS_ALTITUDE);
	TELEMETRY_UART::init (uart_utils::get_bitrate (eeprom_read_byte (EEPROM_ADDR_BITRATE), MSP_DEFAULT_BITRATE));
	requests::init ();
}

// TODO flight modes

bool update ()
{
	// parsing
	bool updated = false;

	while (receive (parse))
	{
		bool changed = true;
		bool was_armed;

#ifdef DEBUG
		dbg_printf_p (PSTR ("G %u\n"), buffer.cmd);
#endif

		switch (buffer.cmd)
		{
			case multiwii::MSP_STATUS:
				was_armed = status::armed;
				status::armed = buffer.payload.status.flags & multiwii::BOX_ARM;
				if (!was_armed && status::armed)
					home::fix ();
				connection_timeout = update_time + MSP_CONNECTION_TIMEOUT;
				status::connection = status::CONNECTED;
				requests::update (buffer.payload.status.sensors);
				mag_enabled = buffer.payload.status.sensors & _BV (multiwii::SENSOR_BIT_MAG);
				break;
			case multiwii::MSP_RC:
				memcpy (input::channels, buffer.payload.rc.channels, sizeof (input::channels));
				break;
			case multiwii::MSP_RAW_GPS:
				gps::satellites = buffer.payload.raw_gps.satellites;
				if (!buffer.payload.raw_gps.fix)
					gps::state = gps::satellites ? GPS_STATE_FIXING : GPS_STATE_NO_FIX;
				else
					gps::state = gps::satellites < 5 ? GPS_STATE_2D : GPS_STATE_3D;
				gps::latitude  = buffer.payload.raw_gps.latitude / 10000000.0;
				gps::longitude = buffer.payload.raw_gps.longitude / 10000000.0;
				gps::altitude  = buffer.payload.raw_gps.altitude / 10.0;
				gps::speed     = buffer.payload.raw_gps.speed / 10.0;
				gps::heading   = round (buffer.payload.raw_gps.ground_course / 10.0);
				stable::groundspeed = gps::speed;
				if (gps_altitude) stable::update_alt_climb (gps::altitude);
				if (internal_home_calc) home::update ();
				if (!mag_enabled)
				{
					stable::heading = gps::heading;
					stable::heading_source = stable::GPS;
				}
				break;
			case multiwii::MSP_COMP_GPS:
				if (!internal_home_calc)
				{
					home::distance = buffer.payload.comp_gps.home_distance;
					home::direction = buffer.payload.comp_gps.home_direction;
				}
				break;
			case multiwii::MSP_ATTITUDE:
				attitude::roll  = buffer.payload.attitude.roll / 10.0;
				attitude::pitch = buffer.payload.attitude.pitch / 10.0;
				attitude::yaw   = buffer.payload.attitude.yaw;
				break;
			case multiwii::MSP_ANALOG:
#ifndef TELEMETRY_MODULES_ADC_BATTERY
				battery::battery1.set_voltage (buffer.payload.analog.vbat / 10.0, true);
				battery::battery1.amperage = (float) buffer.payload.analog.amperage / amperage_divider;
				battery::battery1.consumed = buffer.payload.analog.consumed;
#endif
#ifndef TELEMETRY_MODULES_ADC_RSSI
				input::rssi = (buffer.payload.analog.rssi + 1) * 100 / 1024;
#endif
				break;
			case multiwii::MSP_ALTITUDE:
				if (!gps_altitude)
				{
					stable::altitude = buffer.payload.altitude.altitude / 100.0;
					stable::climb = buffer.payload.altitude.vario / 100.0;
				}
				break;
			case multiwii::MSP_RAW_IMU:
				if (mag_enabled)
				{
					stable::calc_heading (buffer.payload.raw_imu.mag_x, buffer.payload.raw_imu.mag_y);
					stable::heading_source = stable::INTERNAL_MAG;
				}
				break;
			default:
				changed = false;
		}
		updated |= changed;
		requests::receive (buffer.cmd);
	}

	if (update_time >= connection_timeout && status::connection == status::CONNECTED)
	{
		status::connection = status::DISCONNECTED;
		updated = true;
	}

	requests::run ();

	return updated;
}

}  // namespace msp

}  // namespace modules

}  // namespace telemetry
