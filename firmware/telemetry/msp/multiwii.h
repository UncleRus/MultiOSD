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
#ifndef TELEMETRY_MSP_MULTIWII_H_
#define TELEMETRY_MSP_MULTIWII_H_

#include <stdint.h>

#define MULTIWII_SYNC1   '$'
#define MULTIWII_SYNC2   'M'
#define MULTIWII_IN_DIR  '<'
#define MULTIWII_OUT_DIR '>'

namespace multiwii
{

enum command_t
{
	MSP_API_VERSION              = 1,
	MSP_FC_VARIANT               = 2,
	MSP_FC_VERSION               = 3,
	MSP_BOARD_INFO               = 4,
	MSP_BUILD_INFO               = 5,

	// Cleanflight comands
	MSP_MODE_RANGES              = 34,    //out message         Returns all mode ranges
	MSP_SET_MODE_RANGE           = 35,    //in message          Sets a single mode range
	MSP_FEATURE                  = 36,
	MSP_SET_FEATURE              = 37,
	MSP_BOARD_ALIGNMENT          = 38,
	MSP_SET_BOARD_ALIGNMENT      = 39,
	MSP_CURRENT_METER_CONFIG     = 40,
	MSP_SET_CURRENT_METER_CONFIG = 41,
	MSP_MIXER                    = 42,
	MSP_SET_MIXER                = 43,
	MSP_RX_CONFIG                = 44,
	MSP_SET_RX_CONFIG            = 45,
	MSP_LED_COLORS               = 46,
	MSP_SET_LED_COLORS           = 47,
	MSP_LED_STRIP_CONFIG         = 48,
	MSP_SET_LED_STRIP_CONFIG     = 49,
	MSP_RSSI_CONFIG              = 50,
	MSP_SET_RSSI_CONFIG          = 51,
	MSP_ADJUSTMENT_RANGES        = 52,
	MSP_SET_ADJUSTMENT_RANGE     = 53,
	MSP_CF_SERIAL_CONFIG         = 54, // private - only to be used by the configurator, the commands are likely to change
	MSP_SET_CF_SERIAL_CONFIG     = 55, // private
	MSP_VOLTAGE_METER_CONFIG     = 56,
	MSP_SET_VOLTAGE_METER_CONFIG = 57,
	MSP_SONAR_ALTITUDE           = 58, //out message get sonar altitude [cm]
	MSP_PID_CONTROLLER           = 59,
	MSP_SET_PID_CONTROLLER       = 60,
	MSP_ARMING_CONFIG            = 61, //out message         Returns auto_disarm_delay and disarm_kill_switch parameters
	MSP_SET_ARMING_CONFIG        = 62, //in message          Sets auto_disarm_delay and disarm_kill_switch parameters
	MSP_DATAFLASH_SUMMARY        = 70, //out message - get description of dataflash chip
	MSP_DATAFLASH_READ           = 71, //out message - get content of dataflash chip
	MSP_DATAFLASH_ERASE          = 72, //in message - erase dataflash chip
	MSP_LOOP_TIME                = 73, //out message         Returns FC cycle time i.e looptime parameter
	MSP_SET_LOOP_TIME            = 74, //in message          Sets FC cycle time i.e looptime parameter
	MSP_FAILSAFE_CONFIG          = 75, //out message         Returns FC Fail-Safe settings
	MSP_SET_FAILSAFE_CONFIG      = 76, //in message          Sets FC Fail-Safe settings
	MSP_RXFAIL_CONFIG            = 77, //out message         Returns RXFAIL settings
	MSP_SET_RXFAIL_CONFIG        = 78, //in message          Sets RXFAIL settings
	MSP_SDCARD_SUMMARY           = 79, //out message         Get the state of the SD card
	MSP_BLACKBOX_CONFIG          = 80, //out message         Get blackbox settings
	MSP_SET_BLACKBOX_CONFIG      = 81, //in message          Set blackbox settings
	MSP_TRANSPONDER_CONFIG       = 82, //out message         Get transponder settings
	MSP_SET_TRANSPONDER_CONFIG   = 83, //in  message         Set transponder settings
	MSP_LED_STRIP_MODECOLOR      = 86, //out message         Get LED strip mode_color settings
	MSP_SET_LED_STRIP_MODECOLOR  = 87, //in  message         Set LED strip mode_color settings

	// Baseflight MSP commands (if enabled they exist in Cleanflight)
	MSP_RX_MAP                   = 64, //out message get channel map (also returns number of channels total)
	MSP_SET_RX_MAP               = 65, //in message set rx map, numchannels to set comes from MSP_RX_MAP
	MSP_BF_CONFIG                = 66, // DEPRECATED  out message baseflight-specific settings that aren't covered elsewhere
	MSP_SET_BF_CONFIG            = 67, // DEPRECATED  in message baseflight-specific settings save
	MSP_REBOOT                   = 68, //in message reboot settings
	MSP_BF_BUILD_INFO            = 69, // DEPRECATED - Use MSP_BUILD_INFO instead out message build date as well as some space for future expansion

	// Multwii original MSP commands
	MSP_IDENT                    = 100, //out message         mixerMode + multiwii version + protocol version + capability variable
	MSP_STATUS                   = 101, //out message         cycletime & errors_count & sensor present & box activation & current setting number
	MSP_RAW_IMU                  = 102, //out message = 9, DOF
	MSP_SERVO                    = 103, //out message         servos
	MSP_MOTOR                    = 104, //out message         motors
	MSP_RC                       = 105, //out message         rc channels and more
	MSP_RAW_GPS                  = 106, //out message         fix, numsat, lat, lon, alt, speed, ground course
	MSP_COMP_GPS                 = 107, //out message         distance home, direction home
	MSP_ATTITUDE                 = 108, //out message = 2, angles = 1, heading
	MSP_ALTITUDE                 = 109, //out message         altitude, variometer
	MSP_ANALOG                   = 110, //out message         vbat, powermetersum, rssi if available on RX
	MSP_RC_TUNING                = 111, //out message         rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID
	MSP_PID                      = 112, //out message         P I D coeff (9 are used currently)
	MSP_BOX                      = 113, //out message         BOX setup (number is dependant of your setup)
	MSP_MISC                     = 114, //out message         powermeter trig
	MSP_MOTOR_PINS               = 115, //out message         which pins are in use for motors & servos, for GUI
	MSP_BOXNAMES                 = 116, //out message         the aux switch names
	MSP_PIDNAMES                 = 117, //out message         the PID names
	MSP_WP                       = 118, //out message         get a WP, WP# is in the payload, returns (WP#, lat, lon, alt, flags) WP#0-home, WP#16-poshold
	MSP_BOXIDS                   = 119, //out message         get the permanent IDs associated to BOXes
	MSP_SERVO_CONFIGURATIONS     = 120, //out message         All servo configurations.
	MSP_NAV_STATUS               = 121, //out message         Returns navigation status
	MSP_NAV_CONFIG               = 122, //out message         Returns navigation parameters
	MSP_3D                       = 124, //out message         Settings needed for reversible ESCs
	MSP_RC_DEADBAND              = 125, //out message         deadbands for yaw alt pitch roll
	MSP_SENSOR_ALIGNMENT         = 126, //out message         orientation of acc,gyro,mag

	MSP_SET_RAW_RC               = 200, //in message = 8, rc chan
	MSP_SET_RAW_GPS              = 201, //in message          fix, numsat, lat, lon, alt, speed
	MSP_SET_PID                  = 202, //in message          P I D coeff (9 are used currently)
	MSP_SET_BOX                  = 203, //in message          BOX setup (number is dependant of your setup)
	MSP_SET_RC_TUNING            = 204, //in message          rc rate, rc expo, rollpitch rate, yaw rate, dyn throttle PID, yaw expo
	MSP_ACC_CALIBRATION          = 205, //in message          no param
	MSP_MAG_CALIBRATION          = 206, //in message          no param
	MSP_SET_MISC                 = 207, //in message          powermeter trig + = 8, free for future use
	MSP_RESET_CONF               = 208, //in message          no param
	MSP_SET_WP                   = 209, //in message          sets a given WP (WP#,lat, lon, alt, flags)
	MSP_SELECT_SETTING           = 210, //in message          Select Setting Number (0-2)
	MSP_SET_HEAD                 = 211, //in message          define a new heading hold direction
	MSP_SET_SERVO_CONFIGURATION  = 212, //in message          Servo settings
	MSP_SET_MOTOR                = 214, //in message          PropBalance function
	MSP_SET_NAV_CONFIG           = 215, //in message          Sets nav config parameters - write to the eeprom
	MSP_SET_3D                   = 217, //in message          Settings needed for reversible ESCs
	MSP_SET_RC_DEADBAND          = 218, //in message          deadbands for yaw alt pitch roll
	MSP_SET_RESET_CURR_PID       = 219, //in message          resetting the current pid profile to defaults
	MSP_SET_SENSOR_ALIGNMENT     = 220, //in message          set the orientation of the acc,gyro,mag

	MSP_BIND                     = 240, //in message          no param
	MSP_ALARMS                   = 242,

	MSP_EEPROM_WRITE             = 250, //in message          no param

	MSP_DEBUGMSG                 = 253, //out message         debug string buffer
	MSP_DEBUG                    = 254, //out message         debug1,debug2,debug3,debug4

	// Additional commands that are not compatible with MultiWii
	MSP_STATUS_EX                = 150, //out message         cycletime, errors_count, CPU load, sensor present etc
	MSP_UID                      = 160, //out message         Unique device ID
	MSP_GPSSVINFO                = 164, //out message         get Signal Strength (only U-Blox)
	MSP_ACC_TRIM                 = 240, //out message         get acc angle trim values
	MSP_SET_ACC_TRIM             = 239, //in message          set acc angle trim values
	MSP_SERVO_MIX_RULES          = 241, //out message         Returns servo mixer configuration
	MSP_SET_SERVO_MIX_RULE       = 242, //in message          Sets servo mixer configuration
	MSP_SET_4WAY_IF              = 245, //in message          Sets = 4,way interface
};

enum multitype_t
{
	MULTITYPE_UNKNOWN         = 0,
	MULTITYPE_TRI             = 1,
	MULTITYPE_QUADP           = 2,
	MULTITYPE_QUADX           = 3,
	MULTITYPE_BICOPTER        = 4,
	MULTITYPE_GIMBAL          = 5,
	MULTITYPE_Y6              = 6,
	MULTITYPE_HEX6            = 7,
	MULTITYPE_FLYING_WING     = 8,
	MULTITYPE_Y4              = 9,
	MULTITYPE_HEX6X           = 10,
	MULTITYPE_OCTOX8          = 11,
	MULTITYPE_OCTOFLATP       = 12,
	MULTITYPE_OCTOFLATX       = 13,
	MULTITYPE_AIRPLANE        = 14,    // airplane / singlecopter / dualcopter
	MULTITYPE_HELI_120_CCPM   = 15,
	MULTITYPE_HELI_90_DEG     = 16,
	MULTITYPE_VTAIL4          = 17,
	MULTITYPE_HEX6H           = 18,
	MULTITYPE_PPM_TO_SERVO    = 19,    // PPM -> servo relay
	MULTITYPE_DUALCOPTER      = 20,
	MULTITYPE_SINGLECOPTER    = 21,
	MULTITYPE_ATAIL4          = 22,
	MULTITYPE_CUSTOM          = 23,
	MULTITYPE_CUSTOM_AIRPLANE = 24,
	MULTITYPE_CUSTOM_TRI      = 25
};

enum sensor_bit_t
{
	SENSOR_BIT_ACC   = 0,
	SENSOR_BIT_BARO  = 1,
	SENSOR_BIT_MAG   = 2,
	SENSOR_BIT_GPS   = 3,
	SENSOR_BIT_SONAR = 4
};

enum box_bit_t
{
    BOX_ARM       = 0,
    BOX_ANGLE     = 1,
    BOX_HORIZON   = 2,
    BOX_BARO      = 3,
    BOX_VARIO     = 4,
    BOX_MAG       = 5,
    BOX_HEADFREE  = 6,
    BOX_HEADADJ   = 7,
    BOX_CAMSTAB   = 8,
    BOX_CAMTRIG   = 9,
    BOX_GPSHOME   = 10,
    BOX_GPSHOLD   = 11,
    BOX_PASSTHRU  = 12,
    BOX_BEEPERON  = 13,
    BOX_LEDMAX    = 14,
    BOX_LEDLOW    = 15,
    BOX_LLIGHTS   = 16,
    BOX_CALIB     = 17,
    BOX_GOV       = 18,
    BOX_OSD       = 19,
    BOX_TELEMETRY = 20,
    BOX_GTUNE     = 21,
    BOX_SONAR     = 22,
    BOX_SERVO1    = 23,
    BOX_SERVO2    = 24,
    BOX_SERVO3    = 25,
    BOX_BLACKBOX  = 26,
    BOX_FAILSAFE  = 27,
    BOX_AIRMODE   = 28,
};

//////////////////////////////////////////////////////////

struct api_version_t
{
	uint8_t protocol_version;
	uint8_t api_version_major;
	uint8_t api_version_minor;
};

struct fc_variant_t
{
	uint32_t fc_id;
};

struct fc_version_t
{
	uint8_t fc_version_major;
	uint8_t fc_version_minor;
	uint8_t fc_patch_level;
};

struct board_info_t
{
	uint32_t board_id;
	uint16_t hw_revision;
};

struct build_info_t
{
	char build_date [11];
	char build_time [8];
	char git_revision [7];
};

struct ident_t
{
	uint8_t version;
	multitype_t multitype;
	uint8_t protocol_version;
	uint32_t capability;
};

struct status_t
{
	uint16_t cycle_time;        // microseconds
	uint16_t i2c_errors_count;
	uint16_t sensors;           // set of bits sensor_bit_t
	uint32_t flags;             // set of bits values box_bit_t
	uint8_t current_setting;
};

struct raw_imu_t
{
	int16_t accel_x;
	int16_t accel_y;
	int16_t accel_z;
	int16_t gyro_x;
	int16_t gyro_y;
	int16_t gyro_z;
	int16_t mag_x;
	int16_t mag_y;
	int16_t mag_z;
};

struct servo_t
{
	uint16_t servos [8];
};

struct motor_t
{
	uint16_t motors [8];
};

struct rc_t
{
	int16_t channels [8]; // 1000..2000, 18??? channels max
};

struct attitude_t
{
	int16_t roll;   // degrees * 10 (-1800..1800)
	int16_t pitch;  // degrees * 10 (-1800..1800)
	int16_t yaw;    // degrees (-180..180)
};

struct altitude_t
{
	int32_t altitude; // m * 100
	int16_t vario;    // cm/s
};

struct sonar_altitude_t
{
	int32_t altitude;  // m * 100
};

struct analog_t
{
	uint8_t vbat;       // Volts * 10
	uint16_t consumed;  // mAh
	uint16_t rssi;      // 0..1023
	int16_t amperage;   // A * 100 or A * 1000
};

struct raw_gps_t
{
	uint8_t fix;             // 0 - no fix, 2 - fix
	uint8_t satellites;
	uint32_t latitude;       // deg * 10000000
	uint32_t longitude;      // deg * 10000000
	uint16_t altitude;       // m * 10
	uint16_t speed;          // m/s * 10
	uint16_t ground_course;  // degrees * 10, 0..3600
};

struct comp_gps_t
{
	uint16_t home_distance;  // m
	int16_t home_direction;  // degrees, 0..360
	uint8_t update;
};

struct wp_t
{
	uint8_t number;        // currently supported 0 - home, 16 - hold
	uint32_t latitude;     // deg * 10000000
	uint32_t longitude;    // deg * 10000000
	int32_t alt_hold;      // m * 100
	uint16_t heading;      // deg
	uint16_t time_to_stay; // ms
	uint8_t nav_flag;
};

}  // namespace multiwii

#endif /* TELEMETRY_MSP_MULTIWII_H_ */
