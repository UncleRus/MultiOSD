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

#include "release.h"

#include "stddef.h"
#include <string.h>
#include <math.h>
#include "../../../telemetry.h"

UT_NAMESPACE_OPEN

namespace lp150900
{

void handle_manualcontrolcommand ()
{
	ManualControlCommand *obj = (ManualControlCommand *) &buffer.data;
	telemetry::input::throttle   = (int8_t) (obj->Throttle * 100);
	telemetry::input::roll       = (int8_t) (obj->Roll * 100);
	telemetry::input::pitch      = (int8_t) (obj->Pitch * 100);
	telemetry::input::yaw        = (int8_t) (obj->Yaw * 100);
	telemetry::input::collective = (int8_t) (obj->Collective * 100);
	telemetry::input::thrust     = (int8_t) (obj->Thrust * 100);
	memcpy (telemetry::input::channels, obj->Channel, sizeof (obj->Channel));
	telemetry::input::connected = obj->Connected;
	telemetry::input::flight_mode_switch = obj->FlightModeSwitchPosition;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	telemetry::messages::rssi_low = !telemetry::input::connected;
	telemetry::input::rssi = telemetry::input::connected ? 100 : 0;
#endif
}

}  // namespace lp150900

UT_NAMESPACE_CLOSE
