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
#ifndef UAVTALK_DR201702131_FLIGHTSTATUS_H
#define UAVTALK_DR201702131_FLIGHTSTATUS_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace dr201702131
{

#define UAVTALK_DR201702131_FLIGHTSTATUS_OBJID 0xF6AD5B1C

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
	FLIGHTSTATUS_FLIGHTMODE_HORIZON       = 3,
	FLIGHTSTATUS_FLIGHTMODE_AXISLOCK      = 4,
	FLIGHTSTATUS_FLIGHTMODE_VIRTUALBAR    = 5,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED1   = 6,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED2   = 7,
	FLIGHTSTATUS_FLIGHTMODE_STABILIZED3   = 8,
	FLIGHTSTATUS_FLIGHTMODE_AUTOTUNE      = 9,
	FLIGHTSTATUS_FLIGHTMODE_ALTITUDEHOLD  = 10,
	FLIGHTSTATUS_FLIGHTMODE_POSITIONHOLD  = 11,
	FLIGHTSTATUS_FLIGHTMODE_RETURNTOHOME  = 12,
	FLIGHTSTATUS_FLIGHTMODE_PATHPLANNER   = 13,
	FLIGHTSTATUS_FLIGHTMODE_TABLETCONTROL = 14,
	FLIGHTSTATUS_FLIGHTMODE_ACROPLUS      = 15,
	FLIGHTSTATUS_FLIGHTMODE_ACRODYNE      = 16,
	FLIGHTSTATUS_FLIGHTMODE_FAILSAFE      = 17
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

}  // namespace dr201702131

UT_NAMESPACE_CLOSE

#endif // UAVTALK_DR201702131_FLIGHTSTATUS_H
