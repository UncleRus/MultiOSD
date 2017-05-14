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
	input::throttle   = (int8_t) (obj->Throttle * 100);
	input::roll       = (int8_t) (obj->Roll * 100);
	input::pitch      = (int8_t) (obj->Pitch * 100);
	input::yaw        = (int8_t) (obj->Yaw * 100);
	memcpy (input::channels, obj->Channel, sizeof (obj->Channel));
	input::connected = obj->Connected;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
    input::set_rssi (input::connected ? 100 : 0);
#endif
}

}  // namespace lp150900

UT_NAMESPACE_CLOSE
