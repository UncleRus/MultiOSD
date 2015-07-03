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
#ifndef TELEMETRY_UAVTALK_UAVTALK_H_
#define TELEMETRY_UAVTALK_UAVTALK_H_

#ifdef TELEMETRY_MAIN_MODULE_ID
#	error Conflicting modules
#endif
#define TELEMETRY_MAIN_MODULE_ID 'U'

#include <stdint.h>
#include <stddef.h>
#include <avr/pgmspace.h>

// FIXME: real openpilot fw targets
#define UAVTALK_BOARD_CC3D 0
#define UAVTALK_BOARD_REVO 1

#define UAVTALK_VER_141001 0
#define UAVTALK_VER_150202 1

#include "../../config.h"

#define UAVTALK_SYNC 0x3c
#define UAVTALK_HEADER_LEN 0x0a

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

namespace telemetry
{

namespace modules
{

namespace uavtalk
{

const char __name [] PROGMEM = "UAVTalk";

struct header_t
{
	uint8_t sync;
	uint8_t msg_type;
	uint16_t length;
	uint32_t obj_id;
	uint16_t inst_id;

	header_t () :
		sync (UAVTALK_SYNC), msg_type (0), length (UAVTALK_HEADER_LEN), obj_id (0), inst_id (0)
	{}
};

struct message_t
{
	header_t head;
	uint8_t data [255];
	uint8_t crc;

	template <typename T>
	T __attribute__ ((noinline)) get (uint8_t offset)
	{
		T *res = reinterpret_cast<T *> (data + offset);
		return *res;
	}
};

extern message_t buffer;

// load settings
void init ();

// parse messages and update the telemetry
bool update ();

// write default settings to EEPROM
void reset ();

}  // namespace uavtalk

}  // namespace modules

}  // namespace telemetry

#endif /* TELEMETRY_UAVTALK_UAVTALK_H_ */
