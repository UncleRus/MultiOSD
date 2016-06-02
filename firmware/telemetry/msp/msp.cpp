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
#include "commands.h"
#include "../telemetry.h"
#include <math.h>
#include <string.h>
#include "../../lib/uart/uart.h"

namespace telemetry
{

namespace modules
{

namespace msp
{

uint8_t buffer [MSP_BUFFER_SIZE];

enum state_t { s_idle, s_header_start, s_header_m, s_header_arrow, s_header_size, s_header_cmd };

static state_t state;
static uint8_t cmd_size;
static uint8_t cmd;
static uint8_t buf_pos;
static uint8_t crc;

bool parse (uint8_t b)
{
	bool result = false;
	switch (state)
	{
		case s_idle:
			state = b == '$' ? s_header_start : s_idle;
			break;
		case s_header_start:
			state = b == 'M' ? s_header_m : s_idle;
			break;
		case s_header_m:
			state = b == '>' ? s_header_arrow : s_idle;
			break;
		case s_header_arrow:
			if (b <= MSP_BUFFER_SIZE)
			{
				state = s_header_size;
				crc = cmd_size = b;
			}
			else state = s_idle;
			break;
		case s_header_size:
			cmd = b;
			crc ^= b;
			state = s_header_cmd;
			buf_pos = 0;
			break;
		case s_header_cmd:
			if (buf_pos < cmd_size)
			{
				buffer [buf_pos ++] = b;
				crc ^= b;
			}
			else result = crc == b;
			break;
	}
	return result;
}

bool receive ()
{
	uint16_t err = 0;
	do
	{
		uint16_t raw = MSP_UART::receive ();
		err = raw & 0xff00;
		if (!err && parse (raw)) return true;
	}
	while (!err);
	return false;
}

static uint32_t connection_timeout;

bool update ()
{
	bool updated = false;

	while (receive ())
	{
		bool changed = true;

		switch (cmd)
		{
			case MSP_STATUS:
				// TODO: Simulate flight mode!
				telemetry::status::armed = get<uint32_t> (6) & _BV (0);
				telemetry::status::connection = CONNECTION_STATE_CONNECTED;
				connection_timeout = telemetry::ticks + MSP_CONNECTION_TIMEOUT;
				break;
			case MSP_RC:
				memcpy (telemetry::input::channels, buffer, 16);
				telemetry::input::roll = (telemetry::input::channels [0] - 1500) / 10;
				telemetry::input::pitch = (telemetry::input::channels [1] - 1500) / 10;
				telemetry::input::yaw = (telemetry::input::channels [2] - 1500) / 10;
				telemetry::input::throttle = (telemetry::input::channels [3] - 1000) / 10;
				break;
			case MSP_RAW_GPS:
				telemetry::gps::satellites = buffer [1];
				if (buffer [0]) telemetry::gps::state = telemetry::gps::satellites > 4 ? GPS_STATE_3D : GPS_STATE_2D;
				else telemetry::gps::state = telemetry::gps::satellites > 0 ? GPS_STATE_FIXING : GPS_STATE_NO_FIX;
				telemetry::gps::latitude = get<uint32_t> (2) / 10000000.0;
				telemetry::gps::longitude = get<uint32_t> (6) / 10000000.0;
				telemetry::gps::altitude = get<int16_t> (10);
				telemetry::gps::speed = get<uint16_t> (12) / 100.0;
				telemetry::gps::heading = round (get<uint16_t> (12) / 10.0);
				break;
			case MSP_COMP_GPS:
				telemetry::home::distance = get<uint16_t> (0);
				telemetry::home::direction = get<int16_t> (2) + 180;
				break;
			case MSP_ATTITUDE:
				telemetry::attitude::roll = get<int16_t> (0) / 10.0;
				telemetry::attitude::pitch = -get<int16_t> (2) / 10.0;
				telemetry::attitude::yaw = get<uint16_t> (4);
				break;
			case MSP_ALTITUDE:
				telemetry::stable::altitude = get<int32_t> (0) / 100.0;
				telemetry::stable::climb = get<int16_t> (4) / 100.0;
				break;
			case MSP_ANALOG:
				telemetry::battery::voltage = buffer [0] / 10.0;
				telemetry::battery::update_voltage ();
				telemetry::input::rssi = get<uint16_t> (3) / 1023 * 100;
				telemetry::battery::current = get<uint16_t> (5) / 10.0;
				telemetry::battery::update_consumed ();
				break;
			default:
				changed = false;
		}
		updated |= changed;
	}

	if (telemetry::ticks >= connection_timeout && telemetry::status::connection != CONNECTION_STATE_DISCONNECTED)
	{
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{

}

void reset ()
{

}


}  // namespace msp

}  // namespace modules

}  // namespace telemetry
