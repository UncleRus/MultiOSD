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
#include <string.h>

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
#define EEPROM_ADDR_BAUD_RATE          _eeprom_byte (UAVTALK_EEPROM_OFFSET + 4)

const char __opt_mspihc [] PROGMEM = "MSPIHC";
const char __opt_mspad  [] PROGMEM = "MSPAD";
const char __opt_mspga  [] PROGMEM = "MSPGA";
const char __opt_mspbr  [] PROGMEM = "MSPBR";

const ::settings::option_t __settings [] PROGMEM = {
	declare_bool_option   (__opt_mspihc, EEPROM_ADDR_INTERNAL_HOME_CALC),
	declare_uint16_option (__opt_mspad,  EEPROM_ADDR_AMPERAGE_DIVIDER),
	declare_bool_option   (__opt_mspga,  EEPROM_ADDR_GPS_ALTITUDE),
	declare_uint8_option  (__opt_mspbr,  EEPROM_ADDR_BAUD_RATE),
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
	eeprom_update_byte (EEPROM_ADDR_BAUD_RATE, MSP_DEFAULT_BAUD_RATE);
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
		return pos == size;
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
	int8_t sensors_bit;
};

struct request_t
{
	uint8_t cmd;
	uint16_t period; // ms
	uint32_t last;

	void init (const period_t &p)
	{
		cmd = p.cmd;
		period = p.period;
		last = 0;
	}

	void run ()
	{
		if (!period || telemetry::update_time - last < period) return;

		last = telemetry::update_time;

		TELEMETRY_UART::send (MULTIWII_SYNC1);
		TELEMETRY_UART::send (MULTIWII_SYNC2);
		TELEMETRY_UART::send (MULTIWII_IN_DIR);
		TELEMETRY_UART::send (0);   // size
		TELEMETRY_UART::send (cmd); // cmd
		TELEMETRY_UART::send (cmd); // crc = 0 ^ 0 ^ cmd
	}
};

// request rates
const period_t periods [] PROGMEM = {
	{multiwii::MSP_STATUS,   100, -1},                        // 10 Hz
	{multiwii::MSP_RC,       250, -1},                        // 4 Hz
	{multiwii::MSP_RAW_GPS,  200, multiwii::SENSOR_BIT_GPS},  // 5 Hz
	{multiwii::MSP_COMP_GPS, 200, multiwii::SENSOR_BIT_GPS},  // 5 Hz
	{multiwii::MSP_ATTITUDE, 50,  -1},                        // 20 Hz
	{multiwii::MSP_ANALOG,   250, -1},                        // 4 Hz
	{multiwii::MSP_ALTITUDE, 100, multiwii::SENSOR_BIT_BARO}, // 10 Hz
};
const uint8_t requests_len = sizeof (periods) / sizeof (period_t);

request_t requests [requests_len];

void init ()
{
	for (uint8_t i = 0; i < requests_len; i ++)
		requests [i].init (periods [i]);
}

void update (uint16_t msp_sensosrs)
{
	for (uint8_t i = 0; i < requests_len; i ++)
		if (periods [i].sensors_bit >= 0)
			requests [i].period = msp_sensosrs & _BV (periods [i].sensors_bit) ? periods [i].period : 0;
}

void run ()
{
	for (uint8_t i = 0; i < requests_len; i ++)
		requests [i].run ();
}

}  // namespace requests

///////////////////////////////////////////////////////////////////////////////
// main
///////////////////////////////////////////////////////////////////////////////

bool internal_home_calc;
uint16_t amperage_divider;
bool gps_altitude;

void init ()
{
	internal_home_calc = eeprom_read_byte (EEPROM_ADDR_INTERNAL_HOME_CALC);
	amperage_divider = eeprom_read_word (EEPROM_ADDR_AMPERAGE_DIVIDER);
	gps_altitude = eeprom_read_byte (EEPROM_ADDR_GPS_ALTITUDE);
	TELEMETRY_UART::init (uart_utils::get_baudrate (eeprom_read_byte (EEPROM_ADDR_BAUD_RATE), MSP_DEFAULT_BAUD_RATE));
	requests::init ();
}

// TODO connection timeout, flight modes

bool update ()
{
	// do requests
	requests::run ();

	// parsing
	bool updated = false;

	while (receive (parse))
	{
		bool changed = true;
		bool was_armed;

		switch (buffer.cmd)
		{
			case multiwii::MSP_STATUS:
				was_armed = status::armed;
				status::armed = buffer.payload.status.flags & multiwii::BOX_ARM;
				if (!was_armed && status::armed)
					home::fix ();
				status::connection = CONNECTION_STATE_CONNECTED;
				requests::update (buffer.payload.status.sensors);
				break;
			case multiwii::MSP_RC:
				memcpy (input::channels, buffer.payload.rc.channels, sizeof (buffer.payload.rc.channels));
				break;
			case multiwii::MSP_RAW_GPS:
				gps::satellites = buffer.payload.raw_gps.satellites;
				if (!buffer.payload.raw_gps.fix)
					gps::state = gps::satellites ? GPS_STATE_FIXING : GPS_STATE_NO_FIX;
				else
					gps::state = gps::satellites < 4 ? GPS_STATE_2D : GPS_STATE_3D;
				gps::latitude  = buffer.payload.raw_gps.latitude / 10000000.0;
				gps::longitude = buffer.payload.raw_gps.longitude / 10000000.0;
				gps::altitude  = buffer.payload.raw_gps.altitude / 10.0;
				gps::speed     = buffer.payload.raw_gps.speed / 10.0;
				gps::heading   = buffer.payload.raw_gps.ground_course / 10.0;
				stable::groundspeed = gps::speed;
				if (gps_altitude) stable::update_alt_climb (gps::altitude);
				if (internal_home_calc) home::update ();
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
				battery::battery1.voltage = buffer.payload.analog.vbat / 10.0;
				battery::battery1.update (true);
				battery::current = (float) buffer.payload.analog.amperage / amperage_divider;
				battery::consumed = buffer.payload.analog.consumed;
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
			default:
				changed = false;
		}
		updated |= changed;
	}
	return updated;
}

}  // namespace msp

}  // namespace modules

}  // namespace telemetry
