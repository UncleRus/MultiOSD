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

#ifndef UAVTALK_FLIGHTSTATUS_H
#define UAVTALK_FLIGHTSTATUS_H

#define UAVTALK_FLIGHTSTATUS_OBJID 0x8A80EA52
#define UAVTALK_FLIGHTSTATUS_NUMBYTES sizeof (FlightStatusData)

enum FlightStatusDataArmedOptions
{
    FLIGHTSTATUS_ARMED_DISARMED = 0,
    FLIGHTSTATUS_ARMED_ARMING = 1,
    FLIGHTSTATUS_ARMED_ARMED = 2
};

enum FlightStatusFlightModeOptions
{
    FLIGHTSTATUS_FLIGHTMODE_MANUAL = 0,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED1 = 1,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED2 = 2,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED3 = 3,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED4 = 4,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED5 = 5,
    FLIGHTSTATUS_FLIGHTMODE_STABILIZED6 = 6,
    FLIGHTSTATUS_FLIGHTMODE_POSITIONHOLD = 7,
    FLIGHTSTATUS_FLIGHTMODE_COURSELOCK = 8,
    FLIGHTSTATUS_FLIGHTMODE_POSITIONROAM = 9,
    FLIGHTSTATUS_FLIGHTMODE_HOMELEASH = 10,
    FLIGHTSTATUS_FLIGHTMODE_ABSOLUTEPOSITION = 11,
    FLIGHTSTATUS_FLIGHTMODE_RETURNTOBASE = 12,
    FLIGHTSTATUS_FLIGHTMODE_LAND = 13,
    FLIGHTSTATUS_FLIGHTMODE_PATHPLANNER = 14,
    FLIGHTSTATUS_FLIGHTMODE_POI = 15,
    FLIGHTSTATUS_FLIGHTMODE_AUTOCRUISE = 16
};

enum FlightStatusFlightModeAssistOptions
{
    FLIGHTSTATUS_FLIGHTMODEASSIST_NONE = 0,
    FLIGHTSTATUS_FLIGHTMODEASSIST_GPSASSIST_PRIMARYTHRUST = 1,
    FLIGHTSTATUS_FLIGHTMODEASSIST_GPSASSIST = 2
};

enum FlightStatusAssistedControlStateOptions
{
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_PRIMARY = 0,
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_BRAKE = 1,
    FLIGHTSTATUS_ASSISTEDCONTROLSTATE_HOLD = 2
};

enum FlightStatusAssistedThrottleStateOptions
{
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_MANUAL = 0,
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_AUTO = 1,
    FLIGHTSTATUS_ASSISTEDTHROTTLESTATE_AUTOOVERRIDE = 2
};

struct FlightStatusControlChainData
{
    bool Stabilization;
    bool PathFollower;
    bool PathPlanner;
};

struct FlightStatusData
{
	FlightStatusDataArmedOptions Armed;
	FlightStatusFlightModeOptions FlightMode;
	FlightStatusFlightModeAssistOptions FlightModeAssist;
	FlightStatusAssistedControlStateOptions AssistedControlState;
	FlightStatusAssistedThrottleStateOptions AssistedThrottleState;
    FlightStatusControlChainData ControlChain;
};

#endif // UAVTALK_FLIGHTSTATUS_H
