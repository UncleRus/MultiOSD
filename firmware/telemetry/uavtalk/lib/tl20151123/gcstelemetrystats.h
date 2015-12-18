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
#ifndef UAVTALK_TL20151123_GCSTELEMETRYSTATS_H
#define UAVTALK_TL20151123_GCSTELEMETRYSTATS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123_GCSTELEMETRYSTATS_OBJID 0xABC72744

enum GCSTelemetryStatsStatus
{
	GCSTELEMETRYSTATS_STATUS_DISCONNECTED = 0,
	GCSTELEMETRYSTATS_STATUS_HANDSHAKEREQ = 1,
	GCSTELEMETRYSTATS_STATUS_HANDSHAKEACK = 2,
	GCSTELEMETRYSTATS_STATUS_CONNECTED    = 3
};

struct GCSTelemetryStats
{
	float TxDataRate;
	float RxDataRate;
	uint32_t TxFailures;
	uint32_t RxFailures;
	uint32_t TxRetries;
	GCSTelemetryStatsStatus Status;

};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif // UAVTALK_TL20151123_GCSTELEMETRYSTATS_H
