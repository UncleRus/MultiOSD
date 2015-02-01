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

#include <stdint.h>
#include <stddef.h>

// FIXME: real openpilot fw targets
#define UAVTALK_BOARD_CC3D 0
#define UAVTALK_BOARD_REVO 1

#include "../../config.h"

#ifndef UAVTALK_VERSION
#	define UAVTALK_VERSION 0x20
#endif
#ifndef UAVTALK_VERSION_RELEASE
#	define UAVTALK_VERSION_RELEASE 141001
#endif
#ifndef UAVTALK_DEFAULT_BOARD
#	define UAVTALK_DEFAULT_BOARD UAVTALK_BOARD_CC3D
#endif
#ifndef UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL
#	define UAVTALK_GCSTELEMETRYSTATS_UPDATE_INTERVAL 500
#endif

#include "versions.h"

#define UAVTALK_SYNC 0x3c
#define UAVTALK_HEADER_LEN 0x0a

namespace uavtalk
{

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

void init ();
void send (const header_t &head, uint8_t *data = NULL, uint8_t size = 0);
void send_gcs_telemetry_stats (uint8_t status);
void request_object (uint32_t obj_id);
bool update ();

namespace settings
{
	void reset ();
}

}


#endif /* TELEMETRY_UAVTALK_UAVTALK_H_ */
