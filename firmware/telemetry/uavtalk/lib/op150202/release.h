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
#ifndef TELEMETRY_UAVTALK_LIB_OP150202_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_OP150202_RELEASE_H_

#include "../common.h"

#include "attitudestate.h"
#include "barosensor.h"
#include "flightbatterystate.h"
#include "flightstatus.h"
#include "flighttelemetrystats.h"
#include "gcstelemetrystats.h"
#include "gpspositionsensor.h"
#include "gpsvelocitysensor.h"
#include "manualcontrolcommand.h"
#include "positionstate.h"
#include "systemstats.h"

UT_NAMESPACE_OPEN

namespace op150202
{

#define UAVTALK_OP150202 telemetry::modules::uavtalk::op150202

void update_connection ();

void handle_attitudestate ();
void handle_barosensor ();
void handle_flightbatterystate ();
void handle_flightstatus ();
void handle_flighttelemetrystats ();
void handle_gpspositionsensor ();
void handle_gpsvelocitysensor ();
void handle_manualcontrolcommand ();
void handle_positionstate ();
void handle_systemstats ();

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_OP150202_FLIGHTSTATUS_OBJID,         handle_flightstatus},
	{UAVTALK_OP150202_ATTITUDESTATE_OBJID,        handle_attitudestate},
	{UAVTALK_OP150202_BAROSENSOR_OBJID,           handle_barosensor},
	{UAVTALK_OP150202_FLIGHTBATTERYSTATE_OBJID,   handle_flightbatterystate},
	{UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID, handle_flighttelemetrystats},
	{UAVTALK_OP150202_GPSPOSITIONSENSOR_OBJID,    handle_gpspositionsensor},
	{UAVTALK_OP150202_GPSVELOCITYSENSOR_OBJID,    handle_gpsvelocitysensor},
	{UAVTALK_OP150202_MANUALCONTROLCOMMAND_OBJID, handle_manualcontrolcommand},
	{UAVTALK_OP150202_POSITIONSTATE_OBJID,        handle_positionstate},
	{UAVTALK_OP150202_SYSTEMSTATS_OBJID,          handle_systemstats},
	{_UT_TIMEOUT_OBJID,                           update_connection}
};

}  // namespace op150202

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_OP150202_RELEASE_H_ */
