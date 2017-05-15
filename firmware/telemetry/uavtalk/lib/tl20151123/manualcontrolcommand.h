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
#ifndef UAVTALK_TL20151123_MANUALCONTROLCOMMAND_H
#define UAVTALK_TL20151123_MANUALCONTROLCOMMAND_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123_MANUALCONTROLCOMMAND_OBJID 0xD3CE626C

enum ManualControlCommandChannel
{
    MANUALCONTROLCOMMAND_CHANNEL_THROTTLE   = 0,
    MANUALCONTROLCOMMAND_CHANNEL_ROLL       = 1,
    MANUALCONTROLCOMMAND_CHANNEL_PITCH      = 2,
    MANUALCONTROLCOMMAND_CHANNEL_YAW        = 3,
    MANUALCONTROLCOMMAND_CHANNEL_FLIGHTMODE = 4,
    MANUALCONTROLCOMMAND_CHANNEL_COLLECTIVE = 5,
    MANUALCONTROLCOMMAND_CHANNEL_ACCESSORY0 = 6,
    MANUALCONTROLCOMMAND_CHANNEL_ACCESSORY1 = 7,
    MANUALCONTROLCOMMAND_CHANNEL_ACCESSORY2 = 8,
    MANUALCONTROLCOMMAND_CHANNEL_ARMING     = 9
};

struct ManualControlCommand
{
    float Throttle;
    float Roll;
    float Pitch;
    float Yaw;
    uint32_t RawRssi;
    float Collective;
    int16_t Rssi;
    uint16_t Channel [10];
    bool Connected;
    bool ArmSwitch;
};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif // UAVTALK_TL20151123_MANUALCONTROLCOMMAND_H
