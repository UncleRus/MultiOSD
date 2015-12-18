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
#ifndef UAVTALK_TL20151123_FLIGHTSTATUS_H
#define UAVTALK_TL20151123_FLIGHTSTATUS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123_FLIGHTSTATUS_OBJID 0x9BD52DB6

enum FlightStatusArmed
{
	FLIGHTSTATUS_ARMED_DISARMED = 0,
	FLIGHTSTATUS_ARMED_ARMING   = 1,
	FLIGHTSTATUS_ARMED_ARMED	= 2
};

enum FlightStatusFlightMode
{
	FLIGHTSTATUS_FLIGHTMODE_MANUAL        = 0,
	FLIGHTSTATUS_FLIGHTMODE_ACRO          = 1,
	FLIGHTSTATUS_FLIGHTMODE_LEVELING      = 2,
	FLIGHTSTATUS_FLIGHTMODE_MWRATE        = 3,
	FLIGHTSTATUS_FLIGHTMODE_HORIZON       = 4,
	FLIGHTSTATUS_FLIGHTMODE_AXISLOCK      = 5,
	FLIGHTSTATUS_FLIGHTMODE_VIRTUALBAR    = 6,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED1   = 7,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED2   = 8,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED3   = 9,
	FLIGHTSTATUS_FLIGHTMODE_AUTOTUNE      = 10,
	FLIGHTSTATUS_FLIGHTMODE_ALTITUDEHOLD  = 11,
	FLIGHTSTATUS_FLIGHTMODE_POSITIONHOLD  = 12,
	FLIGHTSTATUS_FLIGHTMODE_RETURNTOHOME  = 13,
	FLIGHTSTATUS_FLIGHTMODE_PATHPLANNER   = 14,
	FLIGHTSTATUS_FLIGHTMODE_TABLETCONTROL = 15,
	FLIGHTSTATUS_FLIGHTMODE_ACROPLUS      = 16
};

enum FlightStatusControlSource
{
	FLIGHTSTATUS_CONTROLSOURCE_GEOFENCE	   = 0,
	FLIGHTSTATUS_CONTROLSOURCE_FAILSAFE	   = 1,
	FLIGHTSTATUS_CONTROLSOURCE_TRANSMITTER = 2,
	FLIGHTSTATUS_CONTROLSOURCE_TABLET      = 3
};

struct FlightStatus
{
	FlightStatusArmed Armed;
	FlightStatusFlightMode FlightMode;
	FlightStatusControlSource ControlSource;
};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif // UAVTALK_TL20151123_FLIGHTSTATUS_H
