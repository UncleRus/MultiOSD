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
#ifndef TELEMETRY_UAVTALK_RELEASES_H_
#define TELEMETRY_UAVTALK_RELEASES_H_

#define UAVTALK_FLIGHTSTATUS_OBJ			0
#define UAVTALK_SYSTEMSTATS_OBJ				1
#define UAVTALK_SYSTEMALARMS_OBJ			2
#define UAVTALK_FLIGHTTELEMETRYSTATS_OBJ	3
#define UAVTALK_GCSTELEMETRYSTATS_OBJ		4
#define UAVTALK_ATTITUDESTATE_OBJ			5
#define UAVTALK_MANUALCONTROLCOMMAND_OBJ	6
#define UAVTALK_GPSPOSITIONSENSOR_OBJ		7
#define UAVTALK_GPSTIME_OBJ					8
#define UAVTALK_GPSVELOCITYSENSOR_OBJ		9
#define UAVTALK_FLIGHTBATTERYSTATE_OBJ		10
#define UAVTALK_BAROSENSOR_OBJ				11
#define UAVTALK_HOMELOCATION_OBJ			12
#define UAVTALK_POSITIONSTATE_OBJ			13

#define _UT_OBJ_CNT 14

#define UAVTALK_REL_141001					0
#define UAVTALK_REL_150202					1

#define _UT_REL_141001 { 0xC5FF2D54, 0x40BFFEFC, 0xBA9B00A4, 0x6737BB5A, 0xCAD1DC0A, 0xD7E0D964, 0x161A2C98, 0x7D26A6E6, 0xD4478084, 0x0BC57454, 0x26962352, 0x48120EA6, 0x6185DC6E, 0x4AFDB658 }
#define _UT_REL_150202 { 0x8A80EA52, 0x40BFFEFC, 0x6B7639EC, 0x6737BB5A, 0xCAD1DC0A, 0xD7E0D964, 0x161A2C98, 0x7D26A6E6, 0xD4478084, 0x0BC57454, 0x26962352, 0x48120EA6, 0x6185DC6E, 0x4AFDB658 }

// FIXME: remove hc offsets!
#define _UT_OFFS_FTS_STATUS					36
#define _UT_OFFS_MCC_CHANNELS				24
#define _UT_OFFS_MCC_CONN					42
#define _UT_OFFS_MCC_FMS					43

namespace telemetry {
namespace modules {
namespace uavtalk {

const uint32_t _ut_releases [2][_UT_OBJ_CNT] PROGMEM = {
	_UT_REL_141001,
	_UT_REL_150202
};

}}}


#endif /* TELEMETRY_UAVTALK_RELEASES_H_ */
