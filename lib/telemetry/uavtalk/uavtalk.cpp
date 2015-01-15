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
#include <string.h>
#include "../../settings/settings.h"
#include "../../uart/uart.h"
#include "../../timer/timer.h"
#include "../telemetry.h"

namespace uavtalk
{

static const uint8_t _crc_table [256] PROGMEM =
{
	0x00, 0x07, 0x0e, 0x09, 0x1c, 0x1b, 0x12, 0x15, 0x38, 0x3f, 0x36, 0x31, 0x24, 0x23, 0x2a, 0x2d,
	0x70, 0x77, 0x7e, 0x79, 0x6c, 0x6b, 0x62, 0x65, 0x48, 0x4f, 0x46, 0x41, 0x54, 0x53, 0x5a, 0x5d,
	0xe0, 0xe7, 0xee, 0xe9, 0xfc, 0xfb, 0xf2, 0xf5, 0xd8, 0xdf, 0xd6, 0xd1, 0xc4, 0xc3, 0xca, 0xcd,
	0x90, 0x97, 0x9e, 0x99, 0x8c, 0x8b, 0x82, 0x85, 0xa8, 0xaf, 0xa6, 0xa1, 0xb4, 0xb3, 0xba, 0xbd,
	0xc7, 0xc0, 0xc9, 0xce, 0xdb, 0xdc, 0xd5, 0xd2, 0xff, 0xf8, 0xf1, 0xf6, 0xe3, 0xe4, 0xed, 0xea,
	0xb7, 0xb0, 0xb9, 0xbe, 0xab, 0xac, 0xa5, 0xa2, 0x8f, 0x88, 0x81, 0x86, 0x93, 0x94, 0x9d, 0x9a,
	0x27, 0x20, 0x29, 0x2e, 0x3b, 0x3c, 0x35, 0x32, 0x1f, 0x18, 0x11, 0x16, 0x03, 0x04, 0x0d, 0x0a,
	0x57, 0x50, 0x59, 0x5e, 0x4b, 0x4c, 0x45, 0x42, 0x6f, 0x68, 0x61, 0x66, 0x73, 0x74, 0x7d, 0x7a,
	0x89, 0x8e, 0x87, 0x80, 0x95, 0x92, 0x9b, 0x9c, 0xb1, 0xb6, 0xbf, 0xb8, 0xad, 0xaa, 0xa3, 0xa4,
	0xf9, 0xfe, 0xf7, 0xf0, 0xe5, 0xe2, 0xeb, 0xec, 0xc1, 0xc6, 0xcf, 0xc8, 0xdd, 0xda, 0xd3, 0xd4,
	0x69, 0x6e, 0x67, 0x60, 0x75, 0x72, 0x7b, 0x7c, 0x51, 0x56, 0x5f, 0x58, 0x4d, 0x4a, 0x43, 0x44,
	0x19, 0x1e, 0x17, 0x10, 0x05, 0x02, 0x0b, 0x0c, 0x21, 0x26, 0x2f, 0x28, 0x3d, 0x3a, 0x33, 0x34,
	0x4e, 0x49, 0x40, 0x47, 0x52, 0x55, 0x5c, 0x5b, 0x76, 0x71, 0x78, 0x7f, 0x6a, 0x6d, 0x64, 0x63,
	0x3e, 0x39, 0x30, 0x37, 0x22, 0x25, 0x2c, 0x2b, 0x06, 0x01, 0x08, 0x0f, 0x1a, 0x1d, 0x14, 0x13,
	0xae, 0xa9, 0xa0, 0xa7, 0xb2, 0xb5, 0xbc, 0xbb, 0x96, 0x91, 0x98, 0x9f, 0x8a, 0x8d, 0x84, 0x83,
	0xde, 0xd9, 0xd0, 0xd7, 0xc2, 0xc5, 0xcc, 0xcb, 0xe6, 0xe1, 0xe8, 0xef, 0xfa, 0xfd, 0xf4, 0xf3
};

//#define _get_crc(x) (pgm_read_byte (&_crc_table [x]))
//#define _get_crc(x) (_crc_table [x])

#define _UTPS_WAIT		0
#define _UTPS_SYNC		1
#define _UTPS_MSG_TYPE	2
#define _UTPS_LENGTH	3
#define _UTPS_OBJID		4
#define _UTPS_INSTID	5
#define _UTPS_DATA		6
#define _UTPS_READY		7

#define _UT_TYPE_OBJ		(UAVTALK_VERSION | 0x00)
#define _UT_TYPE_OBJ_REQ	(UAVTALK_VERSION | 0x01)
#define _UT_TYPE_OBJ_ACK	(UAVTALK_VERSION | 0x02)
#define _UT_TYPE_ACK		(UAVTALK_VERSION | 0x03)
#define _UT_TYPE_NACK		(UAVTALK_VERSION | 0x04)

#define _UT_GCSTELEMETRYSTATS_LENGTH 37
#define	_UT_GCSTELEMETRYSTATS_STATUS 36
//#define	_UT_CHANNELS 9

#define UAVTALK_EEPROM_BATTERY_LOW_VOLTAGE _eeprom_float (UAVTALK_EEPROM_OFFSET)

#if (UAVTALK_BOARD == REVO) && !defined (TELEMETRY_MODULES_ADC_BATTERY)
static float _battery_low_voltage;
#endif

static uint8_t __attribute__ ((noinline)) _get_crc (uint8_t b)
{
	return pgm_read_byte (&_crc_table [b]);
}

void init ()
{
#if (UAVTALK_BOARD == REVO) && !defined (TELEMETRY_MODULES_ADC_BATTERY)
	_battery_low_voltage = eeprom_read_float (UAVTALK_EEPROM_BATTERY_LOW_VOLTAGE);
#endif
}

void send (const header_t &head, uint8_t *data, uint8_t size)
{
	uint8_t crc = 0;
	uint8_t *offset = (uint8_t *) &head;
	for (uint8_t i = 0; i < sizeof (header_t); i ++, offset ++)
	{
		crc = _get_crc (crc ^ *offset);
		uart0::send (*offset);
	}
	for (uint8_t i = 0; i < size; i ++)
	{
		uint8_t value = data ? *(data + i) : 0;
		crc = _get_crc (crc ^ value);
		uart0::send (value);
	}
	uart0::send (crc);
}

void send_gcs_telemetry_stats ()
{
	header_t h;
	h.msg_type = _UT_TYPE_OBJ_ACK;
	h.length = UAVTALK_HEADER_LEN + _UT_GCSTELEMETRYSTATS_LENGTH;
	h.obj_id = UAVTALK_GCSTELEMETRYSTATS_OBJID;
	send (h, NULL, _UT_GCSTELEMETRYSTATS_LENGTH);
}

static uint8_t _state = _UTPS_WAIT;
static uint8_t _crc = 0;
static uint8_t _step = 0;

#define _update_crc(b) { _crc = _get_crc (_crc ^ b); }
#define _receive_byte(v, b) { v |=  b << (_step << 3); _step ++; }

message_t buffer;

bool receive ()
{
	uint16_t raw = uart0::receive ();
	if (raw & 0xff00) return false;

	uint8_t b = (uint8_t) raw;

	switch (_state)
	{
		case _UTPS_WAIT:
			if (b != UAVTALK_SYNC) return false;
			_crc = _get_crc (b);
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
			if (_step == buffer.head.length - UAVTALK_HEADER_LEN) _state = _UTPS_DATA;
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


static uint32_t _last_telemetry_request = 0;
static uint32_t _last_flight_time = 0;

bool update ()
{
	bool updated = false;
	uint32_t ticks = timer::ticks ();

	if (uavtalk::receive ())
	{
		updated = true;
		// got message
		switch (buffer.head.obj_id)
		{
			// connection
			case UAVTALK_FLIGHTTELEMETRYSTATS_OBJID:
				if (buffer.data [_UT_OFFS_FTS_STATUS] != 3)
				{
					telemetry::status::connection = CONNECTION_STATE_ESTABLISHING;
					send_gcs_telemetry_stats ();
				}
				else telemetry::status::connection = CONNECTION_STATE_CONNECTED;
				break;
			case UAVTALK_ATTITUDESTATE_OBJID:
				telemetry::attitude::roll 	= buffer.get<float> (16);
				telemetry::attitude::pitch 	= buffer.get<float> (20);
				telemetry::attitude::yaw 	= buffer.get<float> (24);
				break;
			case UAVTALK_FLIGHTSTATUS_OBJID:
				telemetry::status::armed = (bool) buffer.data [0];
				telemetry::status::flight_mode = buffer.data [1];
				break;
			case UAVTALK_MANUALCONTROLCOMMAND_OBJID:
				telemetry::input::throttle 	= (int16_t) (buffer.get<float> (0) * 100);
				telemetry::input::roll 		= (int16_t) (buffer.get<float> (4) * 100);
				telemetry::input::pitch 	= (int16_t) (buffer.get<float> (8) * 100);
				telemetry::input::yaw 		= (int16_t) (buffer.get<float> (12) * 100);
				telemetry::input::collective = (int16_t) (buffer.get<float> (16) * 100);
#if UAVTALK_VERSION_RELEASE >= 141001
				telemetry::input::thrust 	= (int16_t) (buffer.get<float> (20) * 100);
#endif
				memcpy (telemetry::input::channels, buffer.data + _UT_OFFS_MCC_CHANNELS, INPUT_CHANNELS * sizeof (int16_t));
				break;
			case UAVTALK_GPSPOSITIONSENSOR_OBJID:
				telemetry::gps::latitude 	= buffer.get<int32_t> (0) / 10000000.0;
				telemetry::gps::longitude 	= buffer.get<int32_t> (4) / 10000000.0;
				telemetry::gps::altitude 	= buffer.get<float> (8);
				telemetry::gps::heading 	= buffer.get<float> (16);
				telemetry::gps::speed 		= buffer.get<float> (20);
				telemetry::gps::state 		= buffer.data [36];
				telemetry::gps::sattelites 	= buffer.data [37];
				break;
			case UAVTALK_GPSVELOCITYSENSOR_OBJID:
				telemetry::gps::climb = -buffer.get<float> (8);
				// TODO: north/east
				break;
#if (UAVTALK_BOARD == REVO) && !defined (TELEMETRY_MODULES_ADC_BATTERY)
			case UAVTALK_FLIGHTBATTERYSTATE_OBJID:
				telemetry::battery::voltage = buffer.get<float> (0);
				telemetry::battery::current = buffer.get<float> (4);
				telemetry::battery::consumed = (uint16_t) buffer.get<float> (20);
				telemetry::messages::battery_low = telemetry::battery::voltage < _battery_low_voltage;
				break;
#endif
#if (UAVTALK_BOARD == REVO) && !defined (TELEMETRY_MODULES_BMP085)
			case UAVTALK_BAROSENSOR_OBJID:
				telemetry::barometer::altitude = buffer.get<float> (0);
				break;
#endif
			// TODO: more obj_id
			default:
				updated = false;
		}
	}

	if (telemetry::status::armed && ticks >= _last_flight_time + 1000)
	{
		_last_flight_time = ticks;
		telemetry::status::flight_time = ticks / 1000;
		updated = true;
	}

	if (ticks >= _last_telemetry_request + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL)
	{
		uavtalk::send_gcs_telemetry_stats ();
		_last_telemetry_request = ticks;
	}

	return updated;
}


namespace settings
{

void reset ()
{
#if (UAVTALK_BOARD == REVO) && !defined (TELEMETRY_MODULES_ADC_BATTERY)
	eeprom_write_float (UAVTALK_EEPROM_BATTERY_LOW_VOLTAGE, UAVTALK_DEFAULT_BATTERY_LOW_VOLTAGE);
#endif
}

}

}
