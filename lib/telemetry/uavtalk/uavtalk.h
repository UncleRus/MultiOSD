#ifndef LIB_TELEMETRY_UAVTALK_UAVTALK_H_
#define LIB_TELEMETRY_UAVTALK_UAVTALK_H_

#include <stdint.h>
#include <stddef.h>

#include "../../../config.h"

#ifndef UAVTALK_VERSION
#	define UAVTALK_VERSION 0x20
#endif
#ifndef UAVTALK_VERSION_RELEASE
#	define UAVTALK_VERSION_RELEASE 141001
#endif
#ifndef UAVTALK_BOARD
#	define UAVTALK_BOARD CC3D
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
		sync (UAVTALK_SYNC), msg_type (0), length (), obj_id (0), inst_id (0)
	{}
};

struct message_t
{
	header_t head;
	uint8_t data [255];
	uint8_t crc;

	template <class T>
	inline T get (uint8_t offset)
	{
		T *res = reinterpret_cast<T *> (data + offset);
		return *res;
	}
};

extern message_t buffer;

bool receive ();
void send (const header_t &head, uint8_t *data = NULL, uint8_t size = 0);
void send_gcs_telemetry_stats ();
bool update ();

}


#endif /* LIB_TELEMETRY_UAVTALK_UAVTALK_H_ */
