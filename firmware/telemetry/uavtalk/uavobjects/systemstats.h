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

#ifndef UAVTALK_SYSTEMSTATS_H
#define UAVTALK_SYSTEMSTATS_H

#define UAVTALK_SYSTEMSTATS_OBJID 0x40BFFEFC
#define UAVTALK_SYSTEMSTATS_NUMBYTES sizeof (SystemStatsData)

struct SystemStatsData
{
    uint32_t FlightTime;
    uint32_t HeapRemaining;
    uint32_t EventSystemWarningID;
    uint32_t ObjectManagerCallbackID;
    uint32_t ObjectManagerQueueID;
    uint16_t IRQStackRemaining;
    uint16_t SystemModStackRemaining;
    uint16_t SysSlotsFree;
    uint16_t SysSlotsActive;
    uint16_t UsrSlotsFree;
    uint16_t UsrSlotsActive;
    uint8_t CPULoad;
    int8_t CPUTemp;
};

#endif // UAVTALK_SYSTEMSTATS_H
