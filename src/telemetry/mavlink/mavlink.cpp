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
#include "../telemetry.h"

#include "setup.h"

#define MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL _eeprom_byte (MAVLINK_EEPROM_OFFSET)

namespace telemetry
{

namespace modules
{

namespace mavlink
{

uint8_t internal_battery_level;

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

mavlink_status_t status;

uint32_t connection_timeout = 0;

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
	        mavlink_msg_request_data_stream_send (MAVLINK_COMM_0, message.sysid, message.compid,
	        	pgm_read_byte (&rates::values [i].stream), pgm_read_byte (&rates::values [i].rate), 1);
	}

}  // namespace rates

bool receive ()
{
	uint16_t err = 0;
	do
	{
		uint16_t raw = MAVLINK_UART::receive ();
		err = raw & 0xff00;
		if (!err && mavlink_parse_char (MAVLINK_COMM_0, raw, &message, &status)) return true;
	}
	while (!err);
	return false;
}

bool update ()
{
	bool updated = false;

	while (receive ())
	{
		bool changed = true;
		bool was_armed;
		int16_t current;

		switch (message.msgid)
		{
			case MAVLINK_MSG_ID_HEARTBEAT:
				was_armed = telemetry::status::armed;

				telemetry::status::armed = mavlink_msg_heartbeat_get_base_mode (&message) & MAV_MODE_FLAG_SAFETY_ARMED;
				if (!was_armed && telemetry::status::armed)
					telemetry::home::fix ();

				telemetry::status::flight_mode = mavlink_msg_heartbeat_get_custom_mode (&message);
				telemetry::status::flight_mode_name_p = flight_modes::get (mavlink_msg_heartbeat_get_type (&message));

				connection_timeout = telemetry::ticks + MAVLINK_CONNECTION_TIMEOUT;
				if (telemetry::status::connection != CONNECTION_STATE_CONNECTED)
				{
					telemetry::status::connection = CONNECTION_STATE_CONNECTED;
					rates::setup ();
				}

				break;
			case MAVLINK_MSG_ID_SYSTEM_TIME:
				telemetry::status::flight_time = mavlink_msg_system_time_get_time_boot_ms (&message) / 1000;
				break;
			case MAVLINK_MSG_ID_SYS_STATUS:
				telemetry::battery::voltage = mavlink_msg_sys_status_get_voltage_battery (&message) / 1000.0;
				if (!internal_battery_level)
				{
					telemetry::battery::level = mavlink_msg_sys_status_get_battery_remaining (&message);
					if (telemetry::battery::level == 0xff) // -1 (0xff) means "unknown"
						telemetry::battery::level = 0;
				}
				else
					telemetry::battery::update_voltage ();
				current = mavlink_msg_sys_status_get_current_battery (&message);
				if (current >= 0)
				{
					telemetry::battery::current = current / 100.0;
					telemetry::battery::update_consumed ();
				}
				break;
            case MAVLINK_MSG_ID_ATTITUDE:
            	telemetry::attitude::roll = rad_to_deg (mavlink_msg_attitude_get_roll (&message));
            	telemetry::attitude::pitch = rad_to_deg (mavlink_msg_attitude_get_pitch (&message));
            	telemetry::attitude::yaw = rad_to_deg (mavlink_msg_attitude_get_yaw (&message));
                break;
			case MAVLINK_MSG_ID_GPS_RAW_INT:
				telemetry::gps::state = mavlink_msg_gps_raw_int_get_fix_type (&message);
				telemetry::gps::satellites = mavlink_msg_gps_raw_int_get_satellites_visible (&message);
				telemetry::gps::latitude = mavlink_msg_gps_raw_int_get_lat (&message) / 10000000.0;
				telemetry::gps::longitude = mavlink_msg_gps_raw_int_get_lon (&message) / 10000000.0;
				telemetry::gps::altitude = mavlink_msg_gps_raw_int_get_alt (&message) / 1000.0;
				telemetry::gps::speed = mavlink_msg_gps_raw_int_get_vel (&message) / 100.0;
				telemetry::home::update ();
				break;
			case MAVLINK_MSG_ID_VFR_HUD:
				telemetry::stable::ground_speed = mavlink_msg_vfr_hud_get_groundspeed (&message);
				telemetry::stable::air_speed = mavlink_msg_vfr_hud_get_airspeed (&message);
				telemetry::stable::altitude = mavlink_msg_vfr_hud_get_alt (&message);
				telemetry::stable::climb = mavlink_msg_vfr_hud_get_climb (&message);
				telemetry::stable::heading = mavlink_msg_vfr_hud_get_heading (&message);
				telemetry::input::throttle = mavlink_msg_vfr_hud_get_throttle (&message);
				break;
            case MAVLINK_MSG_ID_RC_CHANNELS_RAW:
            	telemetry::input::channels [0] = mavlink_msg_rc_channels_raw_get_chan1_raw (&message);
            	telemetry::input::channels [1] = mavlink_msg_rc_channels_raw_get_chan2_raw (&message);
            	telemetry::input::channels [2] = mavlink_msg_rc_channels_raw_get_chan3_raw (&message);
            	telemetry::input::channels [3] = mavlink_msg_rc_channels_raw_get_chan4_raw (&message);
            	telemetry::input::channels [4] = mavlink_msg_rc_channels_raw_get_chan5_raw (&message);
            	telemetry::input::channels [5] = mavlink_msg_rc_channels_raw_get_chan6_raw (&message);
            	telemetry::input::channels [6] = mavlink_msg_rc_channels_raw_get_chan7_raw (&message);
            	telemetry::input::channels [7] = mavlink_msg_rc_channels_raw_get_chan8_raw (&message);
            	telemetry::input::rssi = mavlink_msg_rc_channels_raw_get_rssi (&message) * 100 / 255;
            	telemetry::messages::rssi_low = telemetry::input::rssi < 10; // TODO: config
            	break;
            case MAVLINK_MSG_ID_SCALED_PRESSURE:
            	telemetry::barometer::temperature = mavlink_msg_scaled_pressure_get_temperature (&message) / 100.0;
            	telemetry::stable::temperature = telemetry::barometer::temperature;
            	telemetry::barometer::pressure = mavlink_msg_scaled_pressure_get_press_abs (&message) / 100;
            	break;
            // TODO: waypoints stuff (idx, dir, dist), windspeed
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
	internal_battery_level = eeprom_read_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL);
}

void reset ()
{
	eeprom_update_byte (MAVLINK_EEPROM_INTERNAL_BATTERY_LEVEL, MAVLINK_DEFAULT_INTERNAL_BATT_LEVEL);
}

}  // namespace mavlink

}  // namespace modules

}  // namespace telemetry
