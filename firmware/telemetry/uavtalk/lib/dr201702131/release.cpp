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
#include "release.h"

#include "stddef.h"
#include "../../../telemetry.h"

UT_NAMESPACE_OPEN

namespace dr201702131
{

#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate ()
{
	// FIXME: use DetectedCellsCount
	FlightBatteryState *obj = (FlightBatteryState *) &buffer.data;
	battery::battery1.set_voltage (obj->Voltage, true);
	battery::battery1.amperage = obj->Current;
	battery::battery1.consumed = obj->ConsumedEnergy;
}
#endif

}  // namespace dr201702131

UT_NAMESPACE_CLOSE
