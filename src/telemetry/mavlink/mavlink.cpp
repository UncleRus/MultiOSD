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

#include "../../lib/max7456/max7456.h"

#include "setup.h"

#define MAVLINK_EEPROM_FW _eeprom_byte (MAVLINK_EEPROM_OFFSET)
#define MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL _eeprom_byte (MAVLINK_EEPROM_OFFSET + 1)

namespace telemetry
{

namespace modules
{

namespace mavlink
{

static uint8_t _fw;
static uint8_t _int_batt_level;

static mavlink_message_t _msg;
static mavlink_status_t _status;

static uint32_t _connection_timeout = 0;
static uint32_t _battery_consumed_last = 0;

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

namespace flight_modes
{

	namespace apm
	{

		const char fm_0 [] PROGMEM  = "MANU";
		const char fm_1 [] PROGMEM  = "CRCL";
		const char fm_2 [] PROGMEM  = "STAB";
		const char fm_3 [] PROGMEM  = "TRN ";
		const char fm_4 [] PROGMEM  = "ACRO";
		const char fm_5 [] PROGMEM  = "FWBA";
		const char fm_6 [] PROGMEM  = "FWBB";
		const char fm_7 [] PROGMEM  = "CRUI";
		const char fm_8 [] PROGMEM  = "ATUN";
		const char fm_10 [] PROGMEM = "AUTO";
		const char fm_11 [] PROGMEM = "RTL ";
		const char fm_12 [] PROGMEM = "LOIT";
		const char fm_14 [] PROGMEM = "LAND";
		const char fm_15 [] PROGMEM = "GUID";
		const char fm_16 [] PROGMEM = "INIT";

		const char * const values [] PROGMEM = {
			fm_0, fm_1, fm_2, fm_3, fm_4, fm_5, fm_6, fm_7,
			fm_8, NULL, fm_10, fm_11, fm_12, NULL, fm_14, fm_15,
			fm_16
		};

		const uint8_t size = sizeof (values) / sizeof (char *);

	}  // namespace apm

	namespace acm
	{

		const char fm_0 [] PROGMEM  = "STAB";
		const char fm_1 [] PROGMEM  = "ACRO";
		const char fm_2 [] PROGMEM  = "AHLD";
		const char fm_3 [] PROGMEM  = "AUTO";
		const char fm_4 [] PROGMEM  = "GUID";
		const char fm_5 [] PROGMEM  = "LOIT";
		const char fm_6 [] PROGMEM  = "RTL ";
		const char fm_7 [] PROGMEM  = "CRCL";
		const char fm_8 [] PROGMEM  = "POS ";
		const char fm_9 [] PROGMEM  = "LAND";
		const char fm_10 [] PROGMEM = "OFLT";
		const char fm_11 [] PROGMEM = "DRFT";
		const char fm_13 [] PROGMEM = "SPRT";
		const char fm_14 [] PROGMEM = "FLIP";
		const char fm_15 [] PROGMEM = "ATUN";
		const char fm_16 [] PROGMEM = "PHLD";

		const char * const values [] PROGMEM = {
			fm_0, fm_1, fm_2, fm_3, fm_4, fm_5, fm_6, fm_7,
			fm_8, fm_9, fm_10, fm_11, NULL, fm_13, fm_14, fm_15,
			fm_16
		};

		const uint8_t size = sizeof (values) / sizeof (char *);

	}  // namespace acm

	const char *get (uint8_t v_type)
	{
		const char * const *values = NULL;
		uint8_t size = 0;
		switch (v_type)
		{
			case MAV_TYPE_QUADROTOR:
			case MAV_TYPE_HELICOPTER:
			case MAV_TYPE_HEXAROTOR:
			case MAV_TYPE_OCTOROTOR:
			case MAV_TYPE_TRICOPTER:
				values = acm::values;
				size = acm::size;
				break;
			case MAV_TYPE_FIXED_WING:
				values = apm::values;
				size = apm::size;
				break;
		}
		if (!values || telemetry::status::flight_mode >= size) return NULL;
		return (const char *) pgm_read_ptr (&values [telemetry::status::flight_mode]);
	}

}  // namespace flight_modes

namespace rates
{

	struct stream_rate_t
	{
		MAV_DATA_STREAM stream;
		uint8_t rate;
	};

	const stream_rate_t values [] PROGMEM = {
		{MAV_DATA_STREAM_RAW_SENSORS, 10},
		{MAV_DATA_STREAM_EXTENDED_STATUS, 25},
		{MAV_DATA_STREAM_RC_CHANNELS, 5},
		{MAV_DATA_STREAM_POSITION, 15},
		{MAV_DATA_STREAM_EXTRA1, 20},
		{MAV_DATA_STREAM_EXTRA2, 5},
	};

	const uint8_t count = sizeof (values) / sizeof (stream_rate_t);

	void setup ()
	{
		for (uint8_t i = 0; i < rates::count; i ++)
	        mavlink_msg_request_data_stream_send (MAVLINK_COMM_0, _msg.sysid, _msg.compid,
	        	pgm_read_byte (&rates::values [i].stream), pgm_read_byte (&rates::values [i].rate), 1);
	}

}  // namespace rates

bool update ()
{
	bool updated = false;
	uint32_t ticks = timer::ticks ();

	float _batt_current_sum = 0;
	uint8_t _batt_mean_cnt = 0;
	while (_receive ())
	{
		bool changed = true;
		bool _was_armed;
		switch (_msg.msgid)
		{
			case MAVLINK_MSG_ID_HEARTBEAT:
				_was_armed = telemetry::status::armed;

				telemetry::status::armed = mavlink_msg_heartbeat_get_base_mode (&_msg) & MAV_MODE_FLAG_SAFETY_ARMED;
				if (!_was_armed && telemetry::status::armed)
					telemetry::home::fix ();

				telemetry::status::flight_mode = mavlink_msg_heartbeat_get_custom_mode (&_msg);
				telemetry::status::flight_mode_name_p = flight_modes::get (mavlink_msg_heartbeat_get_type (&_msg));

				_connection_timeout = ticks + MAVLINK_CONNECTION_TIMEOUT;
				if (telemetry::status::connection != CONNECTION_STATE_CONNECTED)
				{
					telemetry::status::connection = CONNECTION_STATE_CONNECTED;
					rates::setup ();
				}

				break;
			case MAVLINK_MSG_ID_SYS_STATUS:
				telemetry::battery::voltage = mavlink_msg_sys_status_get_voltage_battery (&_msg) / 1000.0;
				if (!_int_batt_level)
				{
					telemetry::battery::level = mavlink_msg_sys_status_get_battery_remaining (&_msg);
					if (telemetry::battery::level > 100) // -1 (0xff) means "unknown"
						telemetry::battery::level = 0;
				}
				else
					telemetry::battery::update_voltage ();

				{
					float cur_mah = mavlink_msg_sys_status_get_current_battery (&_msg);
					telemetry::battery::current = cur_mah / 100.0;
					_batt_current_sum += cur_mah;
					_batt_mean_cnt ++;
					uint16_t interval = ticks - _battery_consumed_last;
					if (interval >= MAVLINK_BATTERY_CONSUMED_INTERVAL)
					{
						telemetry::battery::consumed += _batt_current_sum / _batt_mean_cnt * interval / 360000;
						_batt_current_sum = 0;
						_batt_mean_cnt = 0;
						_battery_consumed_last = ticks;
					}
				}

				break;
            case MAVLINK_MSG_ID_ATTITUDE:
				telemetry::status::flight_time = mavlink_msg_attitude_get_time_boot_ms (&_msg) / 1000;
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
				telemetry::home::update ();
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
	_fw = eeprom_read_byte (MAVLINK_EEPROM_FW);
	_int_batt_level = eeprom_read_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL);
}

void reset ()
{
	eeprom_update_byte (MAVLINK_EEPROM_FW, MAVLINK_DEFAULT_FW);
	eeprom_update_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL, MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL);
}

}  // namespace mavlink

}  // namespace modules

}  // namespace telemetry
