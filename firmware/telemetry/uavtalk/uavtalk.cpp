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
#include <math.h>
#include "../../settings.h"
#include "../../lib/uart/uart.h"
#include "../telemetry.h"

#include "uavobjects.h"

namespace telemetry
{

namespace modules
{

namespace uavtalk
{

static uint8_t board;
static bool internal_home_calc;

const char _fm_man [] PROGMEM = "MANU";
const char _fm_stab1 [] PROGMEM = "STB1";
const char _fm_stab2 [] PROGMEM = "STB2";
const char _fm_stab3 [] PROGMEM = "STB3";
const char _fm_stab4 [] PROGMEM = "STB4";
const char _fm_stab5 [] PROGMEM = "STB5";
const char _fm_stab6 [] PROGMEM = "STB6";
const char _fm_atune [] PROGMEM = "ATUN";
const char _fm_pos_hold [] PROGMEM = "PHLD";
const char _fm_pos_v_fpv [] PROGMEM = "PVAF";
const char _fm_pos_v_los [] PROGMEM = "PVAL";
const char _fm_pos_v_nsew [] PROGMEM = "PVAN";
const char _fm_rtb [] PROGMEM = "RTH ";
const char _fm_land [] PROGMEM = "LAND";
const char _fm_plan [] PROGMEM = "PLAN";
const char _fm_poi [] PROGMEM = "POI ";
const char _fm_acruise [] PROGMEM = "ACRU";

const char * const fm [] PROGMEM = {
	_fm_man, _fm_stab1, _fm_stab2, _fm_stab3, _fm_stab4, _fm_stab5, _fm_stab6,
	_fm_atune, _fm_pos_hold, _fm_pos_v_fpv, _fm_pos_v_los, _fm_pos_v_nsew,
	_fm_rtb, _fm_land, _fm_plan, _fm_poi, _fm_acruise
};

static const uint8_t crc_table [256] PROGMEM =
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

#define UAVTALK_EEPROM_BOARD _eeprom_byte (UAVTALK_EEPROM_OFFSET)
#define UAVTALK_EEPROM_INTERNAL_HOME_CALC _eeprom_byte (UAVTALK_EEPROM_OFFSET + 1)

static uint8_t __attribute__ ((noinline)) _get_crc (uint8_t b)
{
	return pgm_read_byte (&crc_table [b]);
}

void send (const header_t &head, uint8_t *data = NULL, uint8_t size = 0)
{
	uint8_t crc = 0;
	uint8_t *offset = (uint8_t *) &head;
	for (uint8_t i = 0; i < sizeof (header_t); i ++, offset ++)
	{
		crc = _get_crc (crc ^ *offset);
		UAVTALK_UART::send (*offset);
	}
	for (uint8_t i = 0; i < size; i ++)
	{
		uint8_t value = data ? *(data + i) : 0;
		crc = _get_crc (crc ^ value);
		UAVTALK_UART::send (value);
	}
	UAVTALK_UART::send (crc);
}

void send_gcs_telemetry_stats (GCSTelemetryStatsStatusOptions status)
{
	header_t h;
	GCSTelemetryStatsData data;
	data.Status = status;
	h.msg_type = _UT_TYPE_OBJ_ACK;
	h.length = UAVTALK_HEADER_LEN + UAVTALK_GCSTELEMETRYSTATS_NUMBYTES;
	h.obj_id = UAVTALK_GCSTELEMETRYSTATS_OBJID;
	send (h, (uint8_t *) &data, UAVTALK_GCSTELEMETRYSTATS_NUMBYTES);
}

void request_object (uint32_t obj_id)
{
	header_t h;
	h.msg_type = _UT_TYPE_OBJ_REQ;
	h.length = UAVTALK_HEADER_LEN;
	h.obj_id = obj_id;
	send (h, NULL, 0);
}

static uint8_t _state = _UTPS_WAIT;
static uint8_t _crc = 0;
static uint8_t _step = 0;

#define _update_crc(b) { _crc = _get_crc (_crc ^ b); }
#define _receive_byte(v, b) { v |= ((uint32_t) b) << (_step << 3); _step ++; }

message_t buffer;

bool parse (uint8_t b)
{
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

static uint32_t telemetry_request_timeout = 0;
static uint32_t connection_timeout = 0;

inline uint8_t fts_respond (uint8_t state)
{
	if (state == FLIGHTTELEMETRYSTATS_STATUS_DISCONNECTED)
	{
		send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_HANDSHAKEREQ);
		return CONNECTION_STATE_ESTABLISHING;
	}

	if (state == FLIGHTTELEMETRYSTATS_STATUS_HANDSHAKEACK)
	{
		send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
		request_object (UAVTALK_FLIGHTSTATUS_OBJID);
	}

	return CONNECTION_STATE_CONNECTED;
}

#ifdef UAVTALK_FAKE_GPS
struct _gps_data_t
{
	float latitude, longitude, altitude;
};
static const _gps_data_t _fake_gps_data [] PROGMEM = {
	{56.864610, 60.638541, 100},
	{56.865202, 60.637607, 110},
	{56.865009, 60.639506, 200},
	{56.864053, 60.639131, 50},
	{56.864164, 60.637275, 80},
};
static uint8_t _fake_idx = 0;
static uint32_t _fake_gps_update_timeout;
#endif

bool update ()
{
	bool updated = false;

	while (uavtalk::receive ())
	{
		// got message
		bool changed = true;
		bool was_armed;

		switch (buffer.head.obj_id)
		{
			case UAVTALK_FLIGHTSTATUS_OBJID:
			{
				FlightStatusData *obj = (FlightStatusData *) buffer.data;
				was_armed = telemetry::status::armed;
				telemetry::status::armed = obj->Armed > FLIGHTSTATUS_ARMED_DISARMED;
				telemetry::status::flight_mode = obj->FlightMode;
				telemetry::status::flight_mode_name_p = (const char *) pgm_read_ptr (&fm [telemetry::status::flight_mode]);
				// fix home if armed on CC3D
				if ((board == UAVTALK_BOARD_CC3D || internal_home_calc) && !was_armed && telemetry::status::armed)
					telemetry::home::fix ();
				break;
			}
			case UAVTALK_FLIGHTTELEMETRYSTATS_OBJID:
			{
				FlightTelemetryStatsData *obj = (FlightTelemetryStatsData *) buffer.data;
				telemetry::status::connection = fts_respond (obj->Status);
				connection_timeout = telemetry::ticks + UAVTALK_CONNECTION_TIMEOUT;
				break;
			}
			case UAVTALK_SYSTEMSTATS_OBJID:
			{
				SystemStatsData *obj = (SystemStatsData *) buffer.data;
				telemetry::status::flight_time = obj->FlightTime / 1000;
				break;
			}
			case UAVTALK_ATTITUDESTATE_OBJID:
			{
				AttitudeStateData *obj = (AttitudeStateData *) buffer.data;
				telemetry::attitude::roll  = obj->Roll;
				telemetry::attitude::pitch = obj->Pitch;
				telemetry::attitude::yaw   = obj->Yaw;
				break;
			}
			case UAVTALK_MANUALCONTROLCOMMAND_OBJID:
			{
				ManualControlCommandData *obj = (ManualControlCommandData *) buffer.data;
				telemetry::input::throttle   = (int8_t) (obj->Throttle * 100);
				telemetry::input::roll       = (int8_t) (obj->Roll * 100);
				telemetry::input::pitch      = (int8_t) (obj->Pitch * 100);
				telemetry::input::yaw        = (int8_t) (obj->Yaw * 100);
				telemetry::input::collective = (int8_t) (obj->Collective * 100);
				telemetry::input::thrust     = (int8_t) (obj->Thrust * 100);
				memcpy (telemetry::input::channels, obj->Channel, sizeof (obj->Channel));
				telemetry::input::connected = obj->Connected;
				telemetry::input::flight_mode_switch = obj->FlightModeSwitchPosition;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
				telemetry::messages::rssi_low = !telemetry::input::connected;
				telemetry::input::rssi = telemetry::input::connected ? 100 : 0;
#endif
				break;
			}
			case UAVTALK_GPSPOSITIONSENSOR_OBJID:
			{
#ifdef UAVTALK_FAKE_GPS
				if (telemetry::ticks < _fake_gps_update_timeout) break;

				_fake_gps_update_timeout = telemetry::ticks + 2000;
				telemetry::gps::latitude 	= pgm_read_float (&_fake_gps_data [_fake_idx].latitude);
				telemetry::gps::longitude 	= pgm_read_float (&_fake_gps_data [_fake_idx].longitude);
				telemetry::gps::altitude 	= pgm_read_float (&_fake_gps_data [_fake_idx].altitude);
				telemetry::gps::speed 		= 8.0;
				telemetry::gps::state 		= GPS_STATE_3D;
				telemetry::gps::satellites 	= 12;
				if (++_fake_idx >= sizeof (_fake_gps_data) / sizeof (_gps_data_t))
					_fake_idx = 0;
#else
				GPSPositionSensorData *obj = (GPSPositionSensorData *) buffer.data;
				telemetry::gps::latitude    = obj->Latitude / 10000000.0;
				telemetry::gps::longitude   = obj->Longitude / 10000000.0;
				telemetry::gps::altitude    = obj->Altitude;
				telemetry::gps::heading     = round (obj->Heading);
				telemetry::stable::ground_speed = telemetry::gps::speed = obj->Groundspeed;
				telemetry::gps::state 		= obj->Status;
				telemetry::gps::satellites 	= obj->Satellites;
#endif
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
				// let's set heading if we don't have mag
				if (board == UAVTALK_BOARD_CC3D)
					telemetry::stable::heading 	= telemetry::gps::heading;
#endif
#if !defined (TELEMETRY_MODULES_I2C_BARO)
				// update stable altitude if we can't get the baro altitude
				if (board == UAVTALK_BOARD_CC3D)
					telemetry::stable::update_alt_climb (telemetry::gps::altitude);
#endif
				// calc home distance/direction based on gps
				if (board == UAVTALK_BOARD_CC3D || internal_home_calc)
					telemetry::home::update ();
				break;
			}
			case UAVTALK_GPSVELOCITYSENSOR_OBJID:
			{
				GPSVelocitySensorData *obj = (GPSVelocitySensorData *) buffer.data;
				telemetry::gps::climb = -obj->Down;
				// TODO: north/east
				break;
			}
			default:
				changed = false;
		}
		updated |= changed;

		if (board == UAVTALK_BOARD_REVO)
		{
			changed = true;
			switch (buffer.head.obj_id)
			{
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
				case UAVTALK_FLIGHTBATTERYSTATE_OBJID:
				{
					FlightBatteryStateData *obj = (FlightBatteryStateData *) buffer.data;
					telemetry::battery::voltage = obj->Voltage;
					telemetry::battery::cells = obj->NbCells;
					telemetry::battery::update_voltage ();
					telemetry::battery::current = obj->Current;
					telemetry::battery::consumed = obj->ConsumedEnergy;
					break;
				}
#endif
#if !defined (TELEMETRY_MODULES_I2C_BARO)
				case UAVTALK_BAROSENSOR_OBJID:
				{
					BaroSensorData *obj = (BaroSensorData *) buffer.data;
					telemetry::barometer::altitude = obj->Altitude;
					telemetry::barometer::pressure = obj->Pressure;
					telemetry::environment::temperature = telemetry::barometer::temperature = obj->Temperature;
					telemetry::stable::update_alt_climb (telemetry::barometer::altitude);
					break;
				}
#endif
				case UAVTALK_POSITIONSTATE_OBJID:
				{
					if (internal_home_calc)
					{
						changed = false;
						break;
					}
					// TODO: update home::distance & home::direction on REVO
					break;
				}
				default:
					changed = false;
			}
			updated |= changed;
		}
	}

	if (telemetry::ticks >= connection_timeout && telemetry::status::connection != CONNECTION_STATE_DISCONNECTED)
	{
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		telemetry::status::armed = false;
		telemetry::status::flight_mode = 0;
		telemetry::status::flight_mode_name_p = NULL;
		updated = true;
	}

	if (telemetry::ticks >= telemetry_request_timeout && telemetry::status::connection == CONNECTION_STATE_CONNECTED)
	{
		uavtalk::send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
		telemetry_request_timeout = telemetry::ticks + UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL;
	}

	return updated;
}


void init ()
{
	board = eeprom_read_byte (UAVTALK_EEPROM_BOARD);
	internal_home_calc = eeprom_read_byte (UAVTALK_EEPROM_INTERNAL_HOME_CALC);
}

void reset ()
{
	eeprom_update_byte (UAVTALK_EEPROM_BOARD, UAVTALK_DEFAULT_BOARD);
	eeprom_update_byte (UAVTALK_EEPROM_INTERNAL_HOME_CALC, UAVTALK_DEFAULT_INTERNAL_HOME_CALC);
}

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry
