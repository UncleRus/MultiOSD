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
#include "uavtalk.h"
#include <avr/pgmspace.h>
#include "../../lib/uart/uart.h"
#include "../../lib/timer/timer.h"
#include "../telemetry.h"
#include "lib/common.h"
#include "../../settings.h"
#include "../../eeprom.h"

namespace telemetry
{

namespace modules
{

namespace uavtalk
{

namespace settings
{

#define EEPROM_ADDR_RELEASE            _eeprom_byte (UAVTALK_EEPROM_OFFSET)
#define EEPROM_ADDR_BAUDRATE           _eeprom_byte (UAVTALK_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_INTERNAL_HOME_CALC _eeprom_byte (UAVTALK_EEPROM_OFFSET + 2)
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
#define EEPROM_ADDR_RSSI_THRESHOLD     _eeprom_byte (UAVTALK_EEPROM_OFFSET + 3)
#endif

const char __opt_utrel [] PROGMEM = "UTREL";
const char __opt_utbr  [] PROGMEM = "UTBR";
const char __opt_utihc [] PROGMEM = "UTIHC";
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
const char __opt_utrlt [] PROGMEM = "UTRLT";
#endif

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option (__opt_utrel, EEPROM_ADDR_RELEASE),
	declare_uint8_option (__opt_utbr,  EEPROM_ADDR_BAUDRATE),
	declare_uint8_option (__opt_utihc, EEPROM_ADDR_INTERNAL_HOME_CALC),
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	declare_uint8_option (__opt_utrlt, EEPROM_ADDR_RSSI_THRESHOLD),
#endif
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_RELEASE, UAVTALK_DEFAULT_RELEASE);
	eeprom_update_byte (EEPROM_ADDR_BAUDRATE, UAVTALK_DEFAULT_BAUDRATE);
	eeprom_update_byte (EEPROM_ADDR_INTERNAL_HOME_CALC, UAVTALK_DEFAULT_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	eeprom_update_byte (EEPROM_ADDR_RSSI_THRESHOLD, UAVTALK_DEFAULT_RSSI_THRESHOLD);
#endif
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

#define _UTPS_WAIT     0
#define _UTPS_SYNC     1
#define _UTPS_MSG_TYPE 2
#define _UTPS_LENGTH   3
#define _UTPS_OBJID    4
#define _UTPS_INSTID   5
#define _UTPS_DATA     6
#define _UTPS_READY    7

uint8_t parser_state = _UTPS_WAIT;
uint8_t parser_crc = 0;
// current byte in every state
uint8_t parser_step = 0;

#define _update_crc(b) { parser_crc = get_crc (parser_crc ^ b); }
#define _receive_byte(v, b) { v |= ((uint32_t) b) << (parser_step << 3); parser_step ++; }

bool parse (uint8_t b)
{
	switch (parser_state)
	{
		case _UTPS_WAIT:
			if (b != UAVTALK_SYNC) return false;
			parser_crc = get_crc (b);
			buffer.head.sync = b;
			buffer.head.length = 0;
			buffer.head.objid = 0;
			buffer.head.instid = 0;
			parser_state = _UTPS_SYNC;
			break;
		case _UTPS_SYNC:
			if ((b & 0xf8) != UAVTALK_VERSION)
			{
				parser_state = _UTPS_WAIT;
				return false;
			}
			_update_crc (b);
			buffer.head.msg_type = b;
			parser_step = 0;
			parser_state = _UTPS_MSG_TYPE;
			break;
		case _UTPS_MSG_TYPE:
			_receive_byte (buffer.head.length, b);
			_update_crc (b);
			if (parser_step == 2)
			{
				if (buffer.head.length < UAVTALK_HEADER_LEN || buffer.head.length > 0xff + UAVTALK_HEADER_LEN)
				{
					parser_state = _UTPS_WAIT;
					return false;
				}
				parser_state = _UTPS_LENGTH;
				parser_step = 0;
			}
			break;
		case _UTPS_LENGTH:
			_receive_byte (buffer.head.objid, b);
			_update_crc (b);
			if (parser_step == 4)
			{
				parser_state = _UTPS_OBJID;
				parser_step = 0;
			}
			break;
		case _UTPS_OBJID:
			_receive_byte (buffer.head.instid, b);
			_update_crc (b);
			if (parser_step == 2)
			{
				parser_state = buffer.head.length == UAVTALK_HEADER_LEN ? _UTPS_DATA : _UTPS_INSTID;
				parser_step = 0;
			}
			break;
		case _UTPS_INSTID:
			_update_crc (b);
			buffer.data [parser_step] = b;
			parser_step ++;
			if (parser_step >= buffer.head.length - UAVTALK_HEADER_LEN) parser_state = _UTPS_DATA;
			break;
		case _UTPS_DATA:
			buffer.crc = b;
			parser_state = _UTPS_READY;
	}

	if (parser_state == _UTPS_READY)
	{
		parser_state = _UTPS_WAIT;
		bool res = buffer.crc == parser_crc;
		if (res && buffer.head.msg_type == _UT_TYPE_OBJ_ACK)
		{
			header_t head;
			head.objid = buffer.head.objid;
			head.msg_type = _UT_TYPE_ACK;
			send (head);
		}
		return res;
	}

	return false;
}

bool receive ()
{
	uint16_t err = 0;
	do
	{
		uint16_t raw = TELEMETRY_UART::receive ();
		err = raw & 0xff00;
		if (!err && parse (raw)) return true;
	}
	while (!err);
	return false;
}

bool update ()
{
	bool updated = false;

	// handle all received messages
	while (uavtalk::receive ())
	{
		uint32_t ticks = timer::ticks ();
		updated |= handle ();
		if (telemetry::status::connection == CONNECTION_STATE_DISCONNECTED)
		{
			telemetry::status::connection = CONNECTION_STATE_ESTABLISHING;
			connection_timeout = ticks + UAVTALK_CONNECTION_TIMEOUT;
		}
		if (ticks >= telemetry_request_timeout && telemetry::status::connection == CONNECTION_STATE_CONNECTED)
		{
			// time to send GCSTelemetryStats to FC
			update_connection ();
			telemetry_request_timeout = ticks + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL;
		}
	}

	if (ticks >= connection_timeout && telemetry::status::connection == CONNECTION_STATE_CONNECTED)
	{
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{
	release = eeprom_read_byte (EEPROM_ADDR_RELEASE);
	internal_home_calc = eeprom_read_byte (EEPROM_ADDR_INTERNAL_HOME_CALC);
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	rssi_low_threshold = eeprom_read_byte (EEPROM_ADDR_RSSI_THRESHOLD);
#endif
	set_release ();
	TELEMETRY_UART::init (uart_utils::get_baudrate (eeprom_read_byte (EEPROM_ADDR_BAUDRATE), UAVTALK_DEFAULT_BAUDRATE));
}

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry
