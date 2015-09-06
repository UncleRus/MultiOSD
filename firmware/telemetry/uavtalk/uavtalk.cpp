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
#include "uavtalk.h"
#include <avr/pgmspace.h>
#include "../../settings.h"
#include "../../lib/uart/uart.h"
#include "../telemetry.h"
#include "lib/common.h"

namespace telemetry
{

namespace modules
{

namespace uavtalk
{

#define _UTPS_WAIT		0
#define _UTPS_SYNC		1
#define _UTPS_MSG_TYPE	2
#define _UTPS_LENGTH	3
#define _UTPS_OBJID		4
#define _UTPS_INSTID	5
#define _UTPS_DATA		6
#define _UTPS_READY		7

#define UAVTALK_EEPROM_BOARD _eeprom_byte (UAVTALK_EEPROM_OFFSET)
#define UAVTALK_EEPROM_RELEASE _eeprom_byte (UAVTALK_EEPROM_OFFSET + 1)
#define UAVTALK_EEPROM_INTERNAL_HOME_CALC _eeprom_byte (UAVTALK_EEPROM_OFFSET + 2)

static uint8_t _state = _UTPS_WAIT;
static uint8_t _crc = 0;
static uint8_t _step = 0;

#define _update_crc(b) { _crc = get_crc (_crc ^ b); }
#define _receive_byte(v, b) { v |= ((uint32_t) b) << (_step << 3); _step ++; }

bool parse (uint8_t b)
{
	switch (_state)
	{
		case _UTPS_WAIT:
			if (b != UAVTALK_SYNC) return false;
			_crc = get_crc (b);
			buffer.head.sync = b;
			buffer.head.length = 0;
			buffer.head.obj_id = 0;
			buffer.head.inst_id = 0;
			_state = _UTPS_SYNC;
			break;
		case _UTPS_SYNC:
			if ((b & 0xf8) != UAVTALK_VERSION)
			{
				_state = _UTPS_WAIT;
				return false;
			}
			_update_crc (b);
			buffer.head.msg_type = b;
			_step = 0;
			_state = _UTPS_MSG_TYPE;
			break;
		case _UTPS_MSG_TYPE:
			_receive_byte (buffer.head.length, b);
			_update_crc (b);
			if (_step == 2)
			{
				if (buffer.head.length < UAVTALK_HEADER_LEN || buffer.head.length > 0xff + UAVTALK_HEADER_LEN)
				{
					_state = _UTPS_WAIT;
					return false;
				}
				_state = _UTPS_LENGTH;
				_step = 0;
			}
			break;
		case _UTPS_LENGTH:
			_receive_byte (buffer.head.obj_id, b);
			_update_crc (b);
			if (_step == 4)
			{
				_state = _UTPS_OBJID;
				_step = 0;
			}
			break;
		case _UTPS_OBJID:
			_receive_byte (buffer.head.inst_id, b);
			_update_crc (b);
			if (_step == 2)
			{
				_state = buffer.head.length == UAVTALK_HEADER_LEN ? _UTPS_DATA : _UTPS_INSTID;
				_step = 0;
			}
			break;
		case _UTPS_INSTID:
			_update_crc (b);
			buffer.data [_step] = b;
			_step ++;
			if (_step >= buffer.head.length - UAVTALK_HEADER_LEN) _state = _UTPS_DATA;
			break;
		case _UTPS_DATA:
			buffer.crc = b;
			_state = _UTPS_READY;
	}

	if (_state == _UTPS_READY)
	{
		_state = _UTPS_WAIT;
		bool res = buffer.crc == _crc;
		if (res && buffer.head.msg_type == _UT_TYPE_OBJ_ACK)
		{
			header_t head;
			head.obj_id = buffer.head.obj_id;
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
		uint16_t raw = UAVTALK_UART::receive ();
		err = raw & 0xff00;
		if (!err && parse (raw)) return true;
	}
	while (!err);
	return false;
}

bool update ()
{
	bool updated = false;

	while (uavtalk::receive ())
		updated |= handle ();

	if (telemetry::ticks >= connection_timeout && telemetry::status::connection != CONNECTION_STATE_DISCONNECTED)
	{
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	if (telemetry::ticks >= telemetry_request_timeout && telemetry::status::connection == CONNECTION_STATE_CONNECTED)
	{
		update_connection ();
		telemetry_request_timeout = telemetry::ticks + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL;
	}

	return updated;
}


void init ()
{
	board = eeprom_read_byte (UAVTALK_EEPROM_BOARD);
	release = eeprom_read_byte (UAVTALK_EEPROM_RELEASE);
	internal_home_calc = eeprom_read_byte (UAVTALK_EEPROM_INTERNAL_HOME_CALC);
	set_release ();
}

void reset ()
{
	eeprom_update_byte (UAVTALK_EEPROM_BOARD, UAVTALK_DEFAULT_BOARD);
	eeprom_update_byte (UAVTALK_EEPROM_RELEASE, UAVTALK_DEFAULT_RELEASE);
	eeprom_update_byte (UAVTALK_EEPROM_INTERNAL_HOME_CALC, UAVTALK_DEFAULT_INTERNAL_HOME_CALC);
}

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry
