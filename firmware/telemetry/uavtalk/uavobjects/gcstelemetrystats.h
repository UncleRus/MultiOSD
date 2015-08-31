/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef UAVTALK_GCSTELEMETRYSTATS_H
#define UAVTALK_GCSTELEMETRYSTATS_H

#define UAVTALK_GCSTELEMETRYSTATS_OBJID 0xCAD1DC0A
#define UAVTALK_GCSTELEMETRYSTATS_NUMBYTES sizeof (GCSTelemetryStatsData)

enum GCSTelemetryStatsStatusOptions
{
    GCSTELEMETRYSTATS_STATUS_DISCONNECTED = 0,
    GCSTELEMETRYSTATS_STATUS_HANDSHAKEREQ = 1,
    GCSTELEMETRYSTATS_STATUS_HANDSHAKEACK = 2,
    GCSTELEMETRYSTATS_STATUS_CONNECTED = 3
};

struct GCSTelemetryStatsData
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
    GCSTelemetryStatsStatusOptions Status;
};

#endif // UAVTALK_GCSTELEMETRYSTATS_H
