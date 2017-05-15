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
#ifndef UAVTALK_LP1609RC1_FLIGHTSTATUS_H
#define UAVTALK_LP1609RC1_FLIGHTSTATUS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace lp1609rc1
{

#define UAVTALK_LP1609RC1_FLIGHTSTATUS_OBJID 0x24D25E28

enum FlightStatusArmed
{
    FLIGHTSTATUS_ARMED_DISARMED = 0,
    FLIGHTSTATUS_ARMED_ARMING   = 1,
    FLIGHTSTATUS_ARMED_ARMED    = 2
};

enum FlightStatusFlightMode
{
    FLIGHTSTATUS_FLIGHTMODE_MANUAL           = 0,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED1      = 1,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED2      = 2,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED3      = 3,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED4      = 4,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED5      = 5,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED6      = 6,
    FLIGHTSTATUS_FLIGHTMODE_POSITIONHOLD     = 7,
    FLIGHTSTATUS_FLIGHTMODE_COURSELOCK       = 8,
    FLIGHTSTATUS_FLIGHTMODE_POSITIONROAM     = 9,
    FLIGHTSTATUS_FLIGHTMODE_HOMELEASH        = 10,
    FLIGHTSTATUS_FLIGHTMODE_ABSOLUTEPOSITION = 11,
    FLIGHTSTATUS_FLIGHTMODE_RETURNTOBASE     = 12,
    FLIGHTSTATUS_FLIGHTMODE_LAND             = 13,
    FLIGHTSTATUS_FLIGHTMODE_PATHPLANNER      = 14,
    FLIGHTSTATUS_FLIGHTMODE_POI              = 15,
    FLIGHTSTATUS_FLIGHTMODE_AUTOCRUISE       = 16,
    FLIGHTSTATUS_FLIGHTMODE_AUTOTAKEOFF      = 17,
    FLIGHTSTATUS_FLIGHTMODE_AUTOTUNE         = 18
};

enum FlightStatusFlightModeAssist
{
    FLIGHTSTATUS_FLIGHTMODEASSIST_NONE                    = 0,
    FLIGHTSTATUS_FLIGHTMODEASSIST_GPSASSIST_PRIMARYTHRUST = 1,
    FLIGHTSTATUS_FLIGHTMODEASSIST_GPSASSIST               = 2
};

enum FlightStatusAssistedControlState
{
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_PRIMARY = 0,
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_BRAKE   = 1,
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_HOLD    = 2
};

enum FlightStatusAssistedThrottleState
{
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_MANUAL       = 0,
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_AUTO         = 1,
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_AUTOOVERRIDE = 2
};

struct FlightStatusControlChain
{
    bool Stabilization;
    bool PathFollower;
    bool PathPlanner;
};

struct FlightStatus
{
    FlightStatusArmed Armed;
    FlightStatusFlightMode FlightMode;
    bool AlwaysStabilizeWhenArmed;
    FlightStatusFlightModeAssist FlightModeAssist;
    FlightStatusAssistedControlState AssistedControlState;
    FlightStatusAssistedThrottleState AssistedThrottleState;
    FlightStatusControlChain ControlChain;
};

}  // namespace lp1609rc1

UT_NAMESPACE_CLOSE

#endif // UAVTALK_LP1609RC1_FLIGHTSTATUS_H
