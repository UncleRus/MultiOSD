/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
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
#ifndef UAVTALK_LPNEXT_SYSTEMSTATS_H
#define UAVTALK_LPNEXT_SYSTEMSTATS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace lpnext
{

#define UAVTALK_LPNEXT_SYSTEMSTATS_OBJID 0xF1EC270E

struct SystemStats
{
	uint32_t FlightTime;
	uint32_t HeapRemaining;
	uint32_t CPUIdleTicks;
	uint32_t CPUZeroLoadTicks;
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

}  // namespace lpnext

UT_NAMESPACE_CLOSE

#endif // UAVTALK_LPNEXT_SYSTEMSTATS_H
