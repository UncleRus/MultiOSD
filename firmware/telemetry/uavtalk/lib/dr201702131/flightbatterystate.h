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
#ifndef UAVTALK_DR201702131_FLIGHTBATTERYSTATE_H
#define UAVTALK_DR201702131_FLIGHTBATTERYSTATE_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace dr201702131
{

#define UAVTALK_DR201702131_FLIGHTBATTERYSTATE_OBJID 0xCAC62F5E

struct FlightBatteryState
{
	float Voltage;
	float Current;
	float BoardSupplyVoltage;
	float PeakCurrent;
	float AvgCurrent;
	float ConsumedEnergy;
	float EstimatedFlightTime;
	uint8_t DetectedCellCount;
};

}  // namespace dr201702131

UT_NAMESPACE_CLOSE

#endif // UAVTALK_DR201702131_FLIGHTBATTERYSTATE_H
