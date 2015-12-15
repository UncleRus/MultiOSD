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
#ifndef TELEMETRY_UBX_UBLOX_H_
#define TELEMETRY_UBX_UBLOX_H_

#include <stdint.h>

namespace telemetry
{

namespace modules
{

namespace ubx
{

// UBX protocol synchronization bytes
#define UBX_SYNC1 0xb5
#define UBX_SYNC2 0x62

enum fix_type_t
{
	f_no_fix = 0,
	f_dead_reckoing,
	f_2d,
	f_3d,
	gf_gps_dead_reckoing,
	gf_gps_time
};

#define UBX_STATUS_FLAGS_GPSFIX_OK _BV(0)
#define UBX_STATUS_FLAGS_DIFFSOLN  _BV(1)
#define UBX_STATUS_FLAGS_WKNSET    _BV(2)
#define UBX_STATUS_FLAGS_TOWSET    _BV(3)

#define _WORD(h,l) ((h << 8) | l)

// Classes
#define UBX_CLASS_NAV     0x01
#define UBX_CLASS_RXM     0x02
#define UBX_CLASS_ACK     0x05
#define UBX_CLASS_CFG     0x06
#define UBX_CLASS_MON     0x0A
#define UBX_CLASS_AID     0x0B

// Message IDs
// NAV
#define UBX_ID_NAV_POSLLH    0x02
#define UBX_ID_NAV_STATUS    0x03
#define UBX_ID_NAV_DOP       0x04
#define UBX_ID_NAV_SOL       0x06
#define UBX_ID_NAV_VELNED    0x12
#define UBX_ID_NAV_TIMEUTC   0x21
#define UBX_ID_NAV_SVINFO    0x30
#define UBX_ID_NAV_PVT       0x07
#define UBX_ID_NAV_AOPSTATUS 0x60
#define UBX_ID_NAV_CLOCK     0x22
#define UBX_ID_NAV_DGPS      0x31
#define UBX_ID_NAV_POSECEF   0x01
#define UBX_ID_NAV_SBAS      0x32
#define UBX_ID_NAV_TIMEGPS   0x20
#define UBX_ID_NAV_VELECEF   0x11
// RXM
#define UBX_ID_RXM_ALM       0x30
#define UBX_ID_RXM_EPH       0x31
#define UBX_ID_RXM_RAW       0x10
#define UBX_ID_RXM_SFRB      0x11
#define UBX_ID_RXM_SVSI      0x20
// ACK
#define UBX_ID_ACK_ACK       0x01
#define UBX_ID_ACK_NAK       0x00
// MON
#define UBX_ID_MON_VER       0x04
#define UBX_ID_MON_HW2       0x0B
#define UBX_ID_MON_HW        0x09
#define UBX_ID_MON_IO        0x02
#define UBX_ID_MON_MSGPP     0x06
#define UBX_ID_MON_RXBUFF    0x07
#define UBX_ID_MON_RXR       0x21
#define UBX_ID_MON_TXBUF     0x08
// CFG
#define UBX_ID_CFG_NAV5      0x24
#define UBX_ID_CFG_RATE      0x08
#define UBX_ID_CFG_MSG       0x01
#define UBX_ID_CFG_CFG       0x09
#define UBX_ID_CFG_SBAS      0x16
#define UBX_ID_CFG_GNSS      0x3E
#define UBX_ID_CFG_PRT       0x00
// AID
#define UBX_ID_AID_ALM       0x0B
#define UBX_ID_AID_ALPSRV    0x32
#define UBX_ID_AID_ALP       0x50
#define UBX_ID_AID_AOP       0x33
#define UBX_ID_AID_DATA      0x10
#define UBX_ID_AID_EPH       0x31
#define UBX_ID_AID_HUI       0x02
#define UBX_ID_AID_INI       0x01
#define UBX_ID_AID_REQ       0x00

///////////////////////////////////////////////////////////////////////////////

// message header
struct header_t
{
    uint8_t cls;
    uint8_t id;
    uint16_t len;

    header_t (uint8_t cls_ = 0, uint8_t id_ = 0, uint16_t len_ = 0)
    	: cls (cls_), id (id_), len (len_)
    {}
};

// Status
struct nav_status_t
{
	uint32_t itow;              // GPS Millisecond Time of Week (ms)
	uint8_t  fix_type;          // GPS fix type
	uint8_t  fix_flags;         // Navigation Status Flags
	uint8_t  fix_status;        // Fix Status Information
	uint8_t  flags;             // Additional navigation output information
	uint32_t time_to_first_fix; // Time to first fix (ms)
	uint32_t uptime;            // Milliseconds since startup/reset (ms)
};

// Geodetic Position Solution
struct nav_posllh_t
{
    uint32_t itow;    // GPS Millisecond Time of Week (ms)
    int32_t  lon;     // Longitude (deg*1e-7)
    int32_t  lat;     // Latitude (deg*1e-7)
    int32_t  alt;     // Height above Ellipsoid (mm)
    int32_t  alt_msl; // Height above mean sea level (mm)
    uint32_t h_acc;   // Horizontal Accuracy Estimate (mm)
    uint32_t v_acc;   // Vertical Accuracy Estimate (mm)
};

// Dilution of precision
struct nav_dop_t
{
    uint32_t itow; // GPS Millisecond Time of Week (ms)
    uint16_t gdop; // Geometric DOP
    uint16_t pdop; // Position DOP
    uint16_t tdop; // Time DOP
    uint16_t vdop; // Vertical DOP
    uint16_t hdop; // Horizontal DOP
    uint16_t ndop; // Northing DOP
    uint16_t edop; // Easting DOP
};

// Navigation solution
struct nav_sol_t
{
    uint32_t itow;       // GPS Millisecond Time of Week (ms)
    int32_t  ftow;       // fractional nanoseconds (ns)
    int16_t  week;       // GPS week
    uint8_t  fix_type;   // GPS fix type
    uint8_t  flags;      // Fix status flags
    int32_t  ecef_x;     // ECEF X coordinate (cm)
    int32_t  ecef_y;     // ECEF Y coordinate (cm)
    int32_t  ecef_z;     // ECEF Z coordinate (cm)
    uint32_t pos_acc;    // 3D Position Accuracy Estimate (cm)
    int32_t  ecef_vx;    // ECEF X coordinate (cm/s)
    int32_t  ecef_vy;    // ECEF Y coordinate (cm/s)
    int32_t  ecef_vz;    // ECEF Z coordinate (cm/s)
    uint32_t speed_acc;  // Speed Accuracy Estimate
    uint16_t pdop;       // Position DOP
    uint8_t  reserved1;  // Reserved
    uint8_t  num_sv;     // Number of SVs used in Nav Solution
    uint32_t reserved2;  // Reserved
};

// North/East/Down velocity
struct nav_velned_t
{
    uint32_t itow;         // ms GPS Millisecond Time of Week
    int32_t  vel_north;    // cm/s NED north velocity
    int32_t  vel_east;     // cm/s NED east velocity
    int32_t  vel_down;     // cm/s NED down velocity
    uint32_t speed;        // cm/s Speed (3-D)
    uint32_t ground_speed; // cm/s Ground Speed (2-D)
    int32_t  heading;      // 1e-5 *deg Heading of motion 2-D
    uint32_t speed_acc;    // cm/s Speed Accuracy Estimate
    uint32_t course_acc;   // 1e-5 *deg Course / Heading Accuracy Estimate
};

struct ack_nack_t
{
	uint8_t cls; // Message class
	uint8_t id;  // Message ID
};

union payload_t
{
	uint8_t data [1];
	nav_posllh_t nav_posllh;
	nav_status_t status;
	nav_dop_t nav_dop;
	nav_sol_t nav_sol;
	nav_velned_t nav_velned;
	ack_nack_t ack_nak;
};

struct crc_t
{
	uint8_t a;
	uint8_t b;

	crc_t (uint8_t a_ = 0, uint8_t b_ = 0)
		: a (a_), b (b_)
	{}

	void update (uint8_t byte)
	{
		a += byte;
		b += a;
	}

	void reset ()
	{
		a = 0;
		b = 0;
	}
};

struct packet_t
{
	header_t header;
	payload_t payload;
	crc_t crc;
};

}  // namespace ubx

}  // namespace modules

}  // namespace telemetry

#endif /* TELEMETRY_UBX_UBLOX_H_ */
