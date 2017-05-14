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

#define UAVTALK_RELEASE_OP150202    0
#define UAVTALK_RELEASE_OP150502    1
#define UAVTALK_RELEASE_LP150900    2
#define UAVTALK_RELEASE_TL20151123  3
#define UAVTALK_RELEASE_LP1609RC1   4
#define UAVTALK_RELEASE_DR201702131 5

#include "../../../config.h"

#define UAVTALK_VERSION 0x20
#define UAVTALK_SYNC 0x3c

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
		sync (UAVTALK_SYNC), msg_type (0), length (0), objid (0), instid (0)
	{}

	header_t (uint8_t msg_type_, uint16_t length_, uint32_t objid_) :
		sync (UAVTALK_SYNC), msg_type (msg_type_), length (length_), objid (objid_), instid (0)
	{}
};

struct message_t
{
	header_t head;
	uint8_t data [255];
	uint8_t crc;
};

struct obj_handler_t
{
	typedef void (* callable_t) ();

	uint32_t objid;
	callable_t handler;
};

struct release_t
{
	bool instid_required;
	uint32_t flightstatus_objid;
	const obj_handler_t *handlers;
	const char * const *fm_names;
	uint8_t fm_count;
};

extern bool internal_home_calc;
#if !defined (TELEMETRY_MODULES_I2C_BARO)
extern bool baro_enabled;
#endif
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
extern bool mag_enabled;
#endif
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
extern bool emulate_rssi;
#endif
extern uint8_t release_idx;
extern release_t release;
extern uint8_t header_len;
extern uint32_t telemetry_request_timeout;
extern uint32_t connection_timeout;

extern message_t buffer;
bool parse (uint8_t b);

uint8_t __attribute__ ((noinline)) get_crc (uint8_t b);
void send (const header_t &head, uint8_t *data = NULL, uint8_t size = 0);
void request_object (uint32_t obj_id);

// flight mode name
const char *get_fm_name_p (uint8_t fm);
// handle current UAVObject
bool handle ();
// timeout, resend GCSTelemetryStats
void update_connection ();
// init
void set_release ();


UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_COMMON_H_ */
