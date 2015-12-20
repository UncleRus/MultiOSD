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
#ifndef UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_H
#define UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace op150202
{

#define UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID 0x6737BB5A

enum FlightTelemetryStatsStatus
{
    FLIGHTTELEMETRYSTATS_STATUS_DISCONNECTED = 0,
    FLIGHTTELEMETRYSTATS_STATUS_HANDSHAKEREQ = 1,
    FLIGHTTELEMETRYSTATS_STATUS_HANDSHAKEACK = 2,
    FLIGHTTELEMETRYSTATS_STATUS_CONNECTED = 3
};

struct FlightTelemetryStats
{
    float TxDataRate;
    uint32_t TxBytes;
    uint32_t TxFailures;
    uint32_t TxRetries;
    float RxDataRate;
    uint32_t RxBytes;
    uint32_t RxFailures;
    uint32_t RxSyncErrors;
    uint32_t RxCrcErrors;
    FlightTelemetryStatsStatus Status;
};

}  // namespace op150202

UT_NAMESPACE_CLOSE

#endif // UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_H
