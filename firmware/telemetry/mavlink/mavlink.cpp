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
#define __STDC_LIMIT_MACROS
#include <stdint.h>

#include "mavlink.h"

#include <math.h>
#include "../../lib/uart/uart.h"
#include "../telemetry.h"
#include "../../settings.h"
#include "../../eeprom.h"

#include "setup.h"

#define _MAVLINK_PX4_MODE_AUTO 4

namespace telemetry
{

namespace modules
{

namespace mavlink
{

namespace settings
{

#define EEPROM_ADDR_INTERNAL_BATTERY_LEVEL _eeprom_byte (MAVLINK_EEPROM_OFFSET)
#define EEPROM_ADDR_RSSI_LOW_THRESHOLD     _eeprom_byte (MAVLINK_EEPROM_OFFSET + 1)
#define EEPROM_ADDR_EMULATE_RSSI           _eeprom_byte (MAVLINK_EEPROM_OFFSET + 2)
#define EEPROM_ADDR_EMULATE_RSSI_CHANNEL   _eeprom_byte (MAVLINK_EEPROM_OFFSET + 3)
#define EEPROM_ADDR_EMULATE_RSSI_THRESHOLD _eeprom_word (MAVLINK_EEPROM_OFFSET + 4)
#define EEPROM_ADDR_BAUDRATE               _eeprom_byte (MAVLINK_EEPROM_OFFSET + 6)
#define EEPROM_ADDR_FC_TYPE                _eeprom_byte (MAVLINK_EEPROM_OFFSET + 7)
#define EEPROM_ADDR_BATTERY_LOW_THRESHOD   _eeprom_byte (MAVLINK_EEPROM_OFFSET + 8)

const char __opt_mlibl [] PROGMEM = "MLIBL";
const char __opt_mlrlt [] PROGMEM = "MLRLT";
const char __opt_mler  [] PROGMEM = "MLER";
const char __opt_mlerc [] PROGMEM = "MLERC";
const char __opt_mlert [] PROGMEM = "MLERT";
const char __opt_mlbr  [] PROGMEM = "MLBR";
const char __opt_mlfc  [] PROGMEM = "MLFC";
const char __opt_mlblt [] PROGMEM = "MLBLT";

const ::settings::option_t __settings [] PROGMEM = {
	declare_uint8_option  (__opt_mlbr,  EEPROM_ADDR_BAUDRATE),
	declare_uint8_option  (__opt_mlibl, EEPROM_ADDR_INTERNAL_BATTERY_LEVEL),
	declare_uint8_option  (__opt_mlrlt, EEPROM_ADDR_RSSI_LOW_THRESHOLD),
	declare_uint8_option  (__opt_mler,  EEPROM_ADDR_EMULATE_RSSI),
	declare_uint8_option  (__opt_mlerc, EEPROM_ADDR_EMULATE_RSSI_CHANNEL),
	declare_uint16_option (__opt_mlert, EEPROM_ADDR_EMULATE_RSSI_THRESHOLD),
	declare_uint8_option  (__opt_mlfc,  EEPROM_ADDR_FC_TYPE),
	declare_uint8_option  (__opt_mlblt, EEPROM_ADDR_BATTERY_LOW_THRESHOD),
};

void init ()
{
	::settings::append_section (__settings, sizeof (__settings) / sizeof (::settings::option_t));
}

void reset ()
{
	eeprom_update_byte (EEPROM_ADDR_INTERNAL_BATTERY_LEVEL, MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL);
	eeprom_update_byte (EEPROM_ADDR_RSSI_LOW_THRESHOLD, MAVLINK_DEFAULT_RSSI_LOW_THRESHOLD);
	eeprom_update_byte (EEPROM_ADDR_EMULATE_RSSI, MAVLINK_DEFAULT_EMULATE_RSSI);
	eeprom_update_byte (EEPROM_ADDR_EMULATE_RSSI_CHANNEL, MAVLINK_DEFAULT_EMULATE_RSSI_CHANNEL);
	eeprom_update_word (EEPROM_ADDR_EMULATE_RSSI_THRESHOLD, MAVLINK_DEFAULT_EMULATE_RSSI_THRESHOLD);
	eeprom_update_byte (EEPROM_ADDR_BAUDRATE, MAVLINK_DEFAULT_BAUDRATE);
	eeprom_update_byte (EEPROM_ADDR_FC_TYPE, MAVLINK_DEFAULT_FC_TYPE);
	eeprom_update_byte (EEPROM_ADDR_BATTERY_LOW_THRESHOD, MAVLINK_DEFAULT_BATTERY_LOW_THRESHOD);
}

}  // namespace settings

///////////////////////////////////////////////////////////////////////////////

uint8_t internal_battery_level;
uint8_t battery_low_threshod;
uint8_t rssi_low_threshold;
bool emulate_rssi;
uint8_t emulate_rssi_channel;
uint16_t emulate_rssi_threshold;
uint8_t fc_type;

mavlink_message_t *msg = &message;
mavlink_status_t status;

uint32_t connection_timeout = 0;
uint32_t _last_battery_update = 0;

// helpers
float __attribute__ ((noinline)) rad_to_deg (float rad)
{
	return rad * 57.2957795131;
}

inline int16_t filter_int16 (int16_t value)
{
	return value == 0x7fff ? 0 : value;
}

inline uint16_t filter_uint16 (uint16_t value)
{
	return value == 0xffff ? 0 : value;
}

inline uint8_t filter_uint8 (uint8_t value)
{
	return value == 0xff ? 0 : value;
}

namespace flight_modes
{

	namespace generic
	{
		// TODO: more realistic modes
		const char fm_0 [] PROGMEM  = "CUST";
		const char fm_1 [] PROGMEM  = "TEST";
		const char fm_2 [] PROGMEM  = "AUTO";
		const char fm_3 [] PROGMEM  = "GUID";
		const char fm_4 [] PROGMEM  = "STAB";
		const char fm_5 [] PROGMEM  = "HIL ";
		const char fm_6 [] PROGMEM  = "MANU";
		const char * const values [] PROGMEM = {
			fm_0, fm_1, fm_2, fm_3, fm_4, fm_5, fm_6
		};

		void update ()
		{
			uint8_t mode = mavlink_msg_heartbeat_get_base_mode (msg);
			if (mode && MAV_MODE_FLAG_TEST_ENABLED) status::flight_mode = 1;
			else if (mode && MAV_MODE_FLAG_HIL_ENABLED) status::flight_mode = 5;
			else if (mode && MAV_MODE_FLAG_AUTO_ENABLED) status::flight_mode = 2;
			else if (mode && MAV_MODE_FLAG_GUIDED_ENABLED) status::flight_mode = 3;
			else if (mode && MAV_MODE_FLAG_STABILIZE_ENABLED) status::flight_mode = 4;
			else if (mode && MAV_MODE_FLAG_MANUAL_INPUT_ENABLED) status::flight_mode = 6;
			else status::flight_mode = 0;

			status::flight_mode_name_p = (const char *) pgm_read_ptr (&values [status::flight_mode]);
		}

	}  // namespace generic

	namespace apm
	{

		const char apm_0 [] PROGMEM  = "MANU";
		const char apm_1 [] PROGMEM  = "CRCL";
		const char apm_2 [] PROGMEM  = "STAB";
		const char apm_3 [] PROGMEM  = "TRN ";
		const char apm_4 [] PROGMEM  = "ACRO";
		const char apm_5 [] PROGMEM  = "FBWA";
		const char apm_6 [] PROGMEM  = "FBWB";
		const char apm_7 [] PROGMEM  = "CRUI";
		const char apm_8 [] PROGMEM  = "ATUN";
		const char apm_10 [] PROGMEM = "AUTO";
		const char apm_11 [] PROGMEM = "RTL ";
		const char apm_12 [] PROGMEM = "LOIT";
		const char apm_15 [] PROGMEM = "GUID";
		const char apm_16 [] PROGMEM = "INIT";
		const char * const apm_values [] PROGMEM = {
			apm_0, apm_1, apm_2, apm_3, apm_4, apm_5, apm_6, apm_7,
			apm_8, NULL, apm_10, apm_11, apm_12, NULL, NULL, apm_15,
			apm_16
		};
		const uint8_t apm_size = sizeof (apm_values) / sizeof (char *);

		const char acm_0 [] PROGMEM  = "STAB";
		const char acm_1 [] PROGMEM  = "ACRO";
		const char acm_2 [] PROGMEM  = "AHLD";
		const char acm_3 [] PROGMEM  = "AUTO";
		const char acm_4 [] PROGMEM  = "GUID";
		const char acm_5 [] PROGMEM  = "LOIT";
		const char acm_6 [] PROGMEM  = "RTL ";
		const char acm_7 [] PROGMEM  = "CRCL";
		const char acm_9 [] PROGMEM  = "LAND";
		const char acm_10 [] PROGMEM = "OFLT";
		const char acm_11 [] PROGMEM = "DRFT";
		const char acm_13 [] PROGMEM = "SPRT";
		const char acm_14 [] PROGMEM = "FLIP";
		const char acm_15 [] PROGMEM = "ATUN";
		const char acm_16 [] PROGMEM = "PHLD";
		const char acm_17 [] PROGMEM = "BRK ";
		const char * const acm_values [] PROGMEM = {
			acm_0, acm_1, acm_2, acm_3, acm_4, acm_5, acm_6, acm_7,
			NULL, acm_9, acm_10, acm_11, NULL, acm_13, acm_14, acm_15,
			acm_16, acm_17
		};
		const uint8_t acm_size = sizeof (acm_values) / sizeof (char *);

		void update ()
		{
			status::flight_mode = mavlink_msg_heartbeat_get_custom_mode (msg);

			const char * const *values = NULL;
			uint8_t size = 0;
			switch (mavlink_msg_heartbeat_get_type (msg))
			{
				case MAV_TYPE_QUADROTOR:
				case MAV_TYPE_HELICOPTER:
				case MAV_TYPE_HEXAROTOR:
				case MAV_TYPE_OCTOROTOR:
				case MAV_TYPE_TRICOPTER:
					values = acm_values;
					size = acm_size;
					break;
				case MAV_TYPE_FIXED_WING:
				case MAV_TYPE_FLAPPING_WING:
					values = apm_values;
					size = apm_size;
					break;
			}
			status::flight_mode_name_p = values && status::flight_mode < size
				? (const char *) pgm_read_ptr (&values [status::flight_mode])
				: NULL;
		}

	}  // namespace apm

	namespace ppz
	{

		// PAPARAZZI

		const char fw_0 [] PROGMEM = "MANU";
		const char fw_1 [] PROGMEM = "AUT1";
		const char fw_2 [] PROGMEM = "AUT2";
		const char fw_3 [] PROGMEM = "HOME";
		const char * const fw_values [] PROGMEM = {
			fw_0, fw_1, fw_2, fw_3
		};
		const uint8_t fw_size = sizeof (fw_values) / sizeof (char *);

		const char q_0 [] PROGMEM = "MANU";
		const char q_1 [] PROGMEM = "STAB";
		const char q_2 [] PROGMEM = "NAV ";
		const char q_3 [] PROGMEM = "HOME";
		const char * const q_values [] PROGMEM = {
			q_0, q_1, q_2, q_3
		};
		const uint8_t q_size = sizeof (q_values) / sizeof (char *);

		void update ()
		{
			uint8_t size = 0;
			const char * const *values = NULL;
			uint8_t mode = mavlink_msg_heartbeat_get_base_mode (msg);

			if (mavlink_msg_heartbeat_get_type (msg) == MAV_TYPE_FIXED_WING)
			{
				size = fw_size;
				values = fw_values;
			}
			else
			{
				size = q_size;
				values = q_values;
			}

			if (mode & MAV_MODE_FLAG_STABILIZE_ENABLED)
				status::flight_mode = 1;
			else if (mode & MAV_MODE_FLAG_MANUAL_INPUT_ENABLED)
				status::flight_mode = 0;
			else if (mode & MAV_MODE_FLAG_GUIDED_ENABLED)
				status::flight_mode = 2;
			else if (mode & MAV_MODE_FLAG_AUTO_ENABLED)
				status::flight_mode = 3;
			else status::flight_mode = 0xff;

			status::flight_mode_name_p = values && status::flight_mode < size
				? (const char *) pgm_read_ptr (&values [status::flight_mode])
				: NULL;
		}

	}  // namespace ppz

	namespace px4
	{

		const char mm_1 [] PROGMEM = "MANU";
		const char mm_2 [] PROGMEM = "ALTC";
		const char mm_3 [] PROGMEM = "POSC";
		const char mm_5 [] PROGMEM = "ACRO";
		const char mm_6 [] PROGMEM = "OFFB";
		const char mm_7 [] PROGMEM = "STAB";

		const char * const mm [] PROGMEM = {
			NULL, mm_1, mm_2, mm_3, NULL, mm_5, mm_6, mm_7
		};

		const uint8_t mm_size = sizeof (mm) / sizeof (char *);

		const char sm_1 [] PROGMEM = "RDY ";
		const char sm_2 [] PROGMEM = "TKOF";
		const char sm_3 [] PROGMEM = "LOIT";
		const char sm_4 [] PROGMEM = "MISN";
		const char sm_5 [] PROGMEM = "RTL ";
		const char sm_6 [] PROGMEM = "LAND";
		const char sm_7 [] PROGMEM = "RTGS";

		const char * const sm [] PROGMEM = {
			NULL, sm_1, sm_2, sm_3, sm_4, sm_5, sm_6, sm_7
		};

		const uint8_t sm_size = sizeof (sm) / sizeof (char *);

		void update ()
		{
			uint16_t mode = mavlink_msg_heartbeat_get_custom_mode (msg);

			// FIXME: rewrite
			// sqeeze it into byte
			status::flight_mode = ((mode >> 4) & 0xf0) | (mode & 0x0f);
			uint8_t main_mode = mode >> 8;
			uint8_t sub_mode = mode & 0xff;

			if (main_mode >= px4::mm_size)
				status::flight_mode_name_p = NULL;
			else if (main_mode != _MAVLINK_PX4_MODE_AUTO)
				status::flight_mode_name_p = (const char *) pgm_read_ptr (&px4::mm [main_mode]);
			else if (sub_mode >= px4::sm_size)
				status::flight_mode_name_p = NULL;
			else
				status::flight_mode_name_p = (const char *) pgm_read_ptr (&px4::sm [sub_mode]);
		}

	}  // namespace px4

	void update ()
	{
		switch (fc_type ? fc_type : mavlink_msg_heartbeat_get_autopilot (msg))
		{
			case MAV_AUTOPILOT_ARDUPILOTMEGA:
				apm::update ();
				break;
			case MAV_AUTOPILOT_PPZ:
				ppz::update ();
				break;
			case MAV_AUTOPILOT_PX4:
				px4::update ();
				break;
			default:
				generic::update ();
				break;
		}
	}
}  // namespace flight_modes

namespace rates
{

	struct stream_rate_t
	{
		MAV_DATA_STREAM stream;
		uint8_t rate;
	};

	// TODO: config
	const stream_rate_t values [] PROGMEM = {
		{MAV_DATA_STREAM_RAW_SENSORS, 10},
		{MAV_DATA_STREAM_EXTENDED_STATUS, 25},
		{MAV_DATA_STREAM_RC_CHANNELS, 5},
		{MAV_DATA_STREAM_POSITION, 5},
		{MAV_DATA_STREAM_EXTRA1, 20},
		{MAV_DATA_STREAM_EXTRA2, 5},
		{MAV_DATA_STREAM_EXTRA3, 5},
	};

	const uint8_t count = sizeof (values) / sizeof (stream_rate_t);

	void setup ()
	{
		for (uint8_t i = 0; i < rates::count; i ++)
			mavlink_msg_request_data_stream_send (MAVLINK_COMM_0, message.sysid, message.compid,
				pgm_read_byte (&rates::values [i].stream), pgm_read_byte (&rates::values [i].rate), 1);
	}

}  // namespace rates

bool parse (uint8_t b)
{
	return mavlink_parse_char (MAVLINK_COMM_0, b, msg, &status);
}

bool update ()
{
	bool updated = false;

	while (receive (parse))
	{
		bool changed = true;
		bool was_armed;

		switch (msg->msgid)
		{
			case MAVLINK_MSG_ID_HEARTBEAT:
				was_armed = status::armed;

				status::armed = mavlink_msg_heartbeat_get_base_mode (msg) & MAV_MODE_FLAG_SAFETY_ARMED;
				// FIXME: use FC home. Home in ArduXXX is waypoint 0
				if (!was_armed && status::armed)
					home::fix ();

				flight_modes::update ();

				connection_timeout = telemetry::update_time + MAVLINK_CONNECTION_TIMEOUT;
				if (status::connection != CONNECTION_STATE_CONNECTED)
				{
					status::connection = CONNECTION_STATE_CONNECTED;
					rates::setup ();
				}

				break;
			case MAVLINK_MSG_ID_SYSTEM_TIME:
				status::flight_time = mavlink_msg_system_time_get_time_boot_ms (msg) / 1000;
				break;
#ifndef TELEMETRY_MODULES_ADC_BATTERY
			case MAVLINK_MSG_ID_SYS_STATUS:
			{
				uint16_t interval = telemetry::update_time - _last_battery_update;
				_last_battery_update += interval;

				battery::battery1.voltage = mavlink_msg_sys_status_get_voltage_battery (msg) / 1000.0;
				if (!internal_battery_level)
				{
					battery::battery1.level = filter_uint8 (mavlink_msg_sys_status_get_battery_remaining (msg));
					battery::battery1.low = battery::battery1.level <= battery_low_threshod;
				}
				else
					battery::battery1.update (true);
				int16_t current = mavlink_msg_sys_status_get_current_battery (msg);
				if (current >= 0)
				{
					battery::current = current / 100.0;
					battery::update_consumed (interval);
				}
				break;
			}
#endif
			case MAVLINK_MSG_ID_ATTITUDE:
				attitude::roll  = rad_to_deg (mavlink_msg_attitude_get_roll (msg));
				attitude::pitch = rad_to_deg (mavlink_msg_attitude_get_pitch (msg));
				attitude::yaw   = rad_to_deg (mavlink_msg_attitude_get_yaw (msg));
				break;
			case MAVLINK_MSG_ID_GPS_RAW_INT:
				gps::state      = (gps_state_t) mavlink_msg_gps_raw_int_get_fix_type (msg);
				gps::satellites = mavlink_msg_gps_raw_int_get_satellites_visible (msg);
				gps::latitude   = mavlink_msg_gps_raw_int_get_lat (msg) / 10000000.0;
				gps::longitude  = mavlink_msg_gps_raw_int_get_lon (msg) / 10000000.0;
				gps::altitude   = mavlink_msg_gps_raw_int_get_alt (msg) / 1000.0;
				gps::speed      = filter_uint16 (mavlink_msg_gps_raw_int_get_vel (msg)) / 100.0;
				// FIXME: FC home
				home::update ();
				break;
			case MAVLINK_MSG_ID_VFR_HUD:
				stable::groundspeed = mavlink_msg_vfr_hud_get_groundspeed (msg);
				stable::airspeed    = mavlink_msg_vfr_hud_get_airspeed (msg);
				stable::altitude    = mavlink_msg_vfr_hud_get_alt (msg);
				stable::climb       = mavlink_msg_vfr_hud_get_climb (msg);
				input::throttle     = mavlink_msg_vfr_hud_get_throttle (msg);
				stable::heading     = mavlink_msg_vfr_hud_get_heading (msg);
				// FIXME: correct heading source, e.g. for Cleanflight MAVLink
				stable::heading_source = stable::HEADING_SOURCE_EXTERNAL_MAG;
				break;
			case MAVLINK_MSG_ID_RC_CHANNELS_SCALED:
				input::roll  = filter_int16 (mavlink_msg_rc_channels_scaled_get_chan1_scaled (msg)) / 100;
				input::pitch = filter_int16 (mavlink_msg_rc_channels_scaled_get_chan2_scaled (msg)) / 100;
				input::yaw   = filter_int16 (mavlink_msg_rc_channels_scaled_get_chan4_scaled (msg)) / 100;
				break;
			case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
				input::connected = true;		// enable switching between screens
				input::channels [0] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan1_raw (msg));
				input::channels [1] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan2_raw (msg));
				input::channels [2] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan3_raw (msg));
				input::channels [3] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan4_raw (msg));
				input::channels [4] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan5_raw (msg));
				input::channels [5] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan6_raw (msg));
				input::channels [6] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan7_raw (msg));
				input::channels [7] = filter_uint16 (mavlink_msg_rc_channels_raw_get_chan8_raw (msg));
#ifndef TELEMETRY_MODULES_ADC_RSSI
				if (!emulate_rssi)
				{
					input::rssi = (uint16_t) mavlink_msg_rc_channels_raw_get_rssi (msg) * 100 / 255;
					input::rssi_low = input::rssi < rssi_low_threshold;
				}
				else
				{
					input::rssi_low = input::channels [emulate_rssi_channel] < emulate_rssi_threshold;
					input::rssi = input::rssi_low ? 100 : 0;
				}
#endif
				break;
			case MAVLINK_MSG_ID_SCALED_PRESSURE:
				barometer::temperature   = mavlink_msg_scaled_pressure_get_temperature (msg) / 100.0;
				environment::temperature = barometer::temperature;
				barometer::pressure      = mavlink_msg_scaled_pressure_get_press_abs (msg) * 100;
				break;
			case MAVLINK_MSG_ID_WIND:
				environment::wind_direction = mavlink_msg_wind_get_direction (msg);
				environment::wind_speed     = mavlink_msg_wind_get_speed (msg);
				break;
			case MAVLINK_MSG_ID_NAV_CONTROLLER_OUTPUT:
				waypoint::set_bearing (mavlink_msg_nav_controller_output_get_target_bearing (msg));
				waypoint::distance = mavlink_msg_nav_controller_output_get_wp_dist (msg);
				break;
			case MAVLINK_MSG_ID_MISSION_CURRENT:
				waypoint::set (mavlink_msg_mission_current_get_seq (msg));
				break;
			default:
				changed = false;
		}
		updated |= changed;
	}

	if (telemetry::update_time >= connection_timeout && status::connection != CONNECTION_STATE_DISCONNECTED)
	{
		status::connection = CONNECTION_STATE_DISCONNECTED;
		updated = true;
	}

	return updated;
}

void init ()
{
	internal_battery_level = eeprom_read_byte (EEPROM_ADDR_INTERNAL_BATTERY_LEVEL);
	rssi_low_threshold = eeprom_read_byte (EEPROM_ADDR_RSSI_LOW_THRESHOLD);
	emulate_rssi = eeprom_read_byte (EEPROM_ADDR_EMULATE_RSSI);
	emulate_rssi_channel = eeprom_read_byte (EEPROM_ADDR_EMULATE_RSSI_CHANNEL);
	emulate_rssi_threshold = eeprom_read_word (EEPROM_ADDR_EMULATE_RSSI_THRESHOLD);
	fc_type = eeprom_read_byte (EEPROM_ADDR_FC_TYPE);
	battery_low_threshod = eeprom_read_byte (EEPROM_ADDR_BATTERY_LOW_THRESHOD);
	TELEMETRY_UART::init (uart_utils::get_baudrate (eeprom_read_byte (EEPROM_ADDR_BAUDRATE), MAVLINK_DEFAULT_BAUDRATE));
}

}  // namespace mavlink

}  // namespace modules

}  // namespace telemetry
