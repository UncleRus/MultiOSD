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
#ifndef TELEMETRY_UAVTALK_LIB_TL20151123_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_TL20151123_RELEASE_H_

#include "../common.h"

#include "attitudeactual.h"
#include "baroaltitude.h"
#include "flightbatterystate.h"
#include "flightstatus.h"
#include "flighttelemetrystats.h"
#include "gcstelemetrystats.h"
#include "gpsposition.h"
#include "gpsvelocity.h"
#include "manualcontrolcommand.h"
#include "nedposition.h"
#include "systemstats.h"
#include "airspeedactual.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123 telemetry::modules::uavtalk::tl20151123

void update_connection ();

void handle_attitudeactual ();
void handle_baroaltitude ();
void handle_flightbatterystate ();
void handle_flightstatus ();
void handle_flighttelemetrystats ();
void handle_gpsposition ();
void handle_gpsvelocity ();
void handle_manualcontrolcommand ();
void handle_nedposition ();
void handle_systemstats ();
void handle_airspeedactual ();

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_TL20151123_FLIGHTSTATUS_OBJID, handle_flightstatus},
	{UAVTALK_TL20151123_ATTITUDEACTUAL_OBJID, handle_attitudeactual},
	{UAVTALK_TL20151123_BAROALTITUDE_OBJID, handle_baroaltitude},
	{UAVTALK_TL20151123_FLIGHTBATTERYSTATE_OBJID, handle_flightbatterystate},
	{UAVTALK_TL20151123_FLIGHTTELEMETRYSTATS_OBJID, handle_flighttelemetrystats},
	{UAVTALK_TL20151123_GPSPOSITION_OBJID, handle_gpsposition},
	{UAVTALK_TL20151123_GPSVELOCITY_OBJID, handle_gpsvelocity},
	{UAVTALK_TL20151123_MANUALCONTROLCOMMAND_OBJID, handle_manualcontrolcommand},
	{UAVTALK_TL20151123_NEDPOSITION_OBJID, handle_nedposition},
	{UAVTALK_TL20151123_SYSTEMSTATS_OBJID, handle_systemstats},
	{UAVTALK_TL20151123_AIRSPEEDACTUAL_OBJID, handle_airspeedactual},
	{_UT_TIMEOUT_OBJID, update_connection}
};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_TL20151123_RELEASE_H_ */
