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
#ifndef UAVTALK_LP150900_MANUALCONTROLCOMMAND_H
#define UAVTALK_LP150900_MANUALCONTROLCOMMAND_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace lp150900
{

#define UAVTALK_LP150900_MANUALCONTROLCOMMAND_OBJID 0xC4107480

struct ManualControlCommand
{
    float Throttle;
    float Roll;
    float Pitch;
    float Yaw;
    float Collective;
    float Thrust;
    uint16_t Channel [10];
    bool Connected;
    uint8_t FlightModeSwitchPosition;
};

}  // namespace lp150900

UT_NAMESPACE_CLOSE

#endif // UAVTALK_LP150900_MANUALCONTROLCOMMAND_H
