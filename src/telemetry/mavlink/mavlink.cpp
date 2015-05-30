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
#include "mavlink.h"

#include "../../settings.h"
#include "../../lib/uart/uart.h"
#include "../../lib/timer/timer.h"
#include "../telemetry.h"

#define MAVLINK_COMM_NUM_BUFFERS 1

//#include "lib/include/mavlink/v1.0/mavlink_types.h"
#include "lib/include/mavlink/v1.0/ardupilotmega/mavlink.h"

#define MAVLINK_EEPROM_BOARD _eeprom_byte (MAVLINK_EEPROM_OFFSET)
#define MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL _eeprom_byte (MAVLINK_EEPROM_OFFSET + 1)

namespace telemetry
{

namespace modules
{

namespace mavlink
{

const char _fm_0 [] PROGMEM  = "MANU";
const char _fm_1 [] PROGMEM  = "CRCL";
const char _fm_2 [] PROGMEM  = "STAB";
const char _fm_3 [] PROGMEM  = "TRN ";
const char _fm_4 [] PROGMEM  = "ACRO";
const char _fm_5 [] PROGMEM  = "FWBA";
const char _fm_6 [] PROGMEM  = "FWBB";
const char _fm_7 [] PROGMEM  = "CRUI";
const char _fm_8 [] PROGMEM  = "ATUN";
const char _fm_10 [] PROGMEM = "AUTO";
const char _fm_11 [] PROGMEM = "RTL ";
const char _fm_12 [] PROGMEM = "LOIT";
const char _fm_14 [] PROGMEM = "LAND";
const char _fm_15 [] PROGMEM = "GUID";
const char _fm_16 [] PROGMEM = "INIT";

const char * const _fm [] PROGMEM = { _fm_0, _fm_1, _fm_2, _fm_3, _fm_4, _fm_5, _fm_6, _fm_7, _fm_8, NULL, _fm_10, _fm_11, _fm_12, NULL, _fm_14, _fm_15, _fm_16 };

#define _FM_COUNT 17

static uint8_t _board;
static uint8_t _int_batt_level;

static mavlink_message_t _msg;
static mavlink_status_t _status;

static uint32_t _connection_timeout = 0;

bool _receive ()
{
	uint16_t err = 0;
	do
	{
		uint16_t raw = MAVLINK_UART::receive ();
		err = raw & 0xff00;
		if (!err && mavlink_parse_char (MAVLINK_COMM_0, raw, &_msg, &_status)) return true;
	}
	while (!err);
	return false;
}

float __attribute__ ((noinline)) rad_to_deg (float rad)
{
	return rad * 57.2957795131;
}

bool update ()
{
	bool updated = false;
	uint32_t ticks = timer::ticks ();

	while (_receive ())
	{
		bool changed = true;
		switch (_msg.msgid)
		{
			case MAVLINK_MSG_ID_HEARTBEAT:
				telemetry::status::armed = mavlink_msg_heartbeat_get_base_mode (&_msg) & _BV (MAV_MODE_FLAG_SAFETY_ARMED);
				telemetry::status::flight_mode = mavlink_msg_heartbeat_get_custom_mode (&_msg);
				telemetry::status::flight_mode_name = telemetry::status::flight_mode >= _FM_COUNT ? NULL : _fm [telemetry::status::flight_mode];
				_connection_timeout = ticks + MAVLINK_CONNECTION_TIMEOUT;
				telemetry::status::connection = CONNECTION_STATE_CONNECTED;
				break;
			case MAVLINK_MSG_ID_SYS_STATUS:
				telemetry::battery::voltage = mavlink_msg_sys_status_get_voltage_battery (&_msg) / 1000.0;
				telemetry::battery::current = mavlink_msg_sys_status_get_current_battery (&_msg) / 100.0;
				if (!_int_batt_level)
					telemetry::battery::level = mavlink_msg_sys_status_get_battery_remaining (&_msg) / 10;
				else
					telemetry::battery::update_voltage ();
				break;
            case MAVLINK_MSG_ID_ATTITUDE:
            	telemetry::attitude::roll = rad_to_deg (mavlink_msg_attitude_get_roll (&_msg));
            	telemetry::attitude::pitch = rad_to_deg (mavlink_msg_attitude_get_pitch (&_msg));
            	telemetry::attitude::yaw = rad_to_deg (mavlink_msg_attitude_get_yaw (&_msg));
                break;
			case MAVLINK_MSG_ID_GPS_RAW_INT:
				telemetry::gps::state = mavlink_msg_gps_raw_int_get_fix_type (&_msg);
				telemetry::gps::satellites = mavlink_msg_gps_raw_int_get_satellites_visible (&_msg);
				telemetry::gps::latitude = mavlink_msg_gps_raw_int_get_lat (&_msg) / 10000000.0;
				telemetry::gps::longitude = mavlink_msg_gps_raw_int_get_lon (&_msg) / 10000000.0;
				telemetry::gps::altitude = mavlink_msg_gps_raw_int_get_alt (&_msg) / 1000.0;
				telemetry::gps::speed = mavlink_msg_gps_raw_int_get_vel (&_msg) / 100.0;
				break;
			case MAVLINK_MSG_ID_VFR_HUD:
				telemetry::stable::ground_speed = mavlink_msg_vfr_hud_get_groundspeed (&_msg);
				telemetry::stable::air_speed = mavlink_msg_vfr_hud_get_airspeed (&_msg);
				telemetry::stable::altitude = mavlink_msg_vfr_hud_get_alt (&_msg);
				telemetry::stable::climb = mavlink_msg_vfr_hud_get_climb (&_msg);
				telemetry::stable::heading = mavlink_msg_vfr_hud_get_heading (&_msg);
				telemetry::input::throttle = mavlink_msg_vfr_hud_get_throttle (&_msg);
				break;
            case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
            	telemetry::input::channels [0] = mavlink_msg_rc_channels_raw_get_chan1_raw (&_msg);
            	telemetry::input::channels [1] = mavlink_msg_rc_channels_raw_get_chan2_raw (&_msg);
            	telemetry::input::channels [2] = mavlink_msg_rc_channels_raw_get_chan3_raw (&_msg);
            	telemetry::input::channels [3] = mavlink_msg_rc_channels_raw_get_chan4_raw (&_msg);
            	telemetry::input::channels [4] = mavlink_msg_rc_channels_raw_get_chan5_raw (&_msg);
            	telemetry::input::channels [5] = mavlink_msg_rc_channels_raw_get_chan6_raw (&_msg);
            	telemetry::input::channels [6] = mavlink_msg_rc_channels_raw_get_chan7_raw (&_msg);
            	telemetry::input::channels [7] = mavlink_msg_rc_channels_raw_get_chan8_raw (&_msg);
            	telemetry::status::rssi = mavlink_msg_rc_channels_raw_get_rssi (&_msg) * 100 / 255;
            	telemetry::messages::rssi_low = telemetry::status::rssi < 10; // TODO: config
            	break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE:
            	telemetry::barometer::temperature = mavlink_msg_scaled_pressure_get_temperature (&_msg) / 100.0;
            	telemetry::stable::temperature = telemetry::barometer::temperature;	// TODO: config
            	telemetry::barometer::pressure = mavlink_msg_scaled_pressure_get_press_abs (&_msg) / 100;
            	// TODO: calculate TEMPERATURE COMPENSATED alt
            	break;
            default:
            	changed = false;
		}
		updated |= changed;
	}

	if (ticks >= _connection_timeout && telemetry::status::connection != CONNECTION_STATE_DISCONNECTED)
	{
		telemetry::status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{
	_board = eeprom_read_byte (MAVLINK_EEPROM_BOARD);
	_int_batt_level = eeprom_read_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL);
}

void reset ()
{
	eeprom_update_byte (MAVLINK_EEPROM_BOARD, MAVLINK_DEFAULT_BOARD);
	eeprom_update_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL, MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL);
}

}  // namespace mavlink

}  // namespace modules

}  // namespace telemetry
