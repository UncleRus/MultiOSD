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
#ifndef TELEMETRY_UAVTALK_LIB_COMMON_H_
#define TELEMETRY_UAVTALK_LIB_COMMON_H_

#include <stddef.h>
#include "../../../lib/pgmspace.h"
#include "../../../lib/uart/uart.h"

#define UAVTALK_BOARD_CC3D 0
#define UAVTALK_BOARD_REVO 1

#define UAVTALK_RELEASE_OP150202   0
#define UAVTALK_RELEASE_OP150502   1
#define UAVTALK_RELEASE_LP150900   2
#define UAVTALK_RELEASE_TL20151123 3

#include "../../../config.h"

#define UAVTALK_VERSION 0x20
#define UAVTALK_SYNC 0x3c
#define UAVTALK_HEADER_LEN 0x0a

#define _UT_TYPE_OBJ		(UAVTALK_VERSION | 0x00)
#define _UT_TYPE_OBJ_REQ	(UAVTALK_VERSION | 0x01)
#define _UT_TYPE_OBJ_ACK	(UAVTALK_VERSION | 0x02)
#define _UT_TYPE_ACK		(UAVTALK_VERSION | 0x03)
#define _UT_TYPE_NACK		(UAVTALK_VERSION | 0x04)

#define _UT_TIMEOUT_OBJID 0

#define UT_NAMESPACE_OPEN namespace telemetry { namespace modules { namespace uavtalk {
#define UT_NAMESPACE_CLOSE }}}

UT_NAMESPACE_OPEN

struct header_t
{
	uint8_t sync;
	uint8_t msg_type;
	uint16_t length;
	uint32_t objid;
	uint16_t instid;

	header_t () :
		sync (UAVTALK_SYNC), msg_type (0), length (UAVTALK_HEADER_LEN), objid (0), instid (0)
	{}
};

struct message_t
{
	header_t head;
	uint8_t data [255];
	uint8_t crc;
};

namespace fm
{

	const char man [] PROGMEM = "MANU";
	const char stab1 [] PROGMEM = "STB1";
	const char stab2 [] PROGMEM = "STB2";
	const char stab3 [] PROGMEM = "STB3";
	const char stab4 [] PROGMEM = "STB4";
	const char stab5 [] PROGMEM = "STB5";
	const char stab6 [] PROGMEM = "STB6";
	const char atune [] PROGMEM = "ATUN";
	const char pos_hold [] PROGMEM = "PHLD";
	const char pos_v_fpv [] PROGMEM = "PVAF";
	const char pos_v_los [] PROGMEM = "PVAL";
	const char pos_v_nsew [] PROGMEM = "PVAN";
	const char rtb [] PROGMEM = "RTH ";
	const char land [] PROGMEM = "LAND";
	const char plan [] PROGMEM = "PLAN";
	const char poi [] PROGMEM = "POI ";
	const char acruise [] PROGMEM = "ACRU";
	const char atakeoff [] PROGMEM = "ATOF";

	const char * const names [] PROGMEM = {
		man, stab1, stab2, stab3, stab4, stab5, stab6,
		atune, pos_hold, pos_v_fpv, pos_v_los, pos_v_nsew,
		rtb, land, plan, poi, acruise, atakeoff
	};

}  // namespace fm

struct obj_handler_t
{
	typedef void (* callable_t) ();

	uint32_t objid;
	callable_t handler;
};

extern uint8_t board;
extern bool internal_home_calc;
extern uint8_t release;
extern uint32_t telemetry_request_timeout;
extern uint32_t connection_timeout;
extern uint32_t fts_objid;

extern message_t buffer;

uint8_t __attribute__ ((noinline)) get_crc (uint8_t b);
void send (const header_t &head, uint8_t *data = NULL, uint8_t size = 0);
void request_object (uint32_t obj_id);

// handle current UAVObject
bool handle ();
// timeout, resend GCSTelemetryStats
void update_connection ();
// init
void set_release ();

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_COMMON_H_ */
