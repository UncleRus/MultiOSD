/*
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
#ifndef TELEMETRY_UAVTALK_LIB_150502_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_150502_RELEASE_H_

#include "../common.h"

#include "flightstatus.h"

#include "../150202/release.h"

UT_NAMESPACE_OPEN

namespace r150502
{

#define UAVTALK_R150502 telemetry::modules::uavtalk::r150502

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_R150202_ATTITUDESTATE_OBJID, UAVTALK_R150202::handle_attitudestate},
	{UAVTALK_R150202_BAROSENSOR_OBJID, UAVTALK_R150202::handle_barosensor},
	{UAVTALK_R150202_FLIGHTBATTERYSTATE_OBJID, UAVTALK_R150202::handle_flightbatterystate},
	{UAVTALK_R150502_FLIGHTSTATUS_OBJID, UAVTALK_R150202::handle_flightstatus},				// new obj_id
	{UAVTALK_R150202_FLIGHTTELEMETRYSTATS_OBJID, UAVTALK_R150202::handle_flighttelemetrystats},
	{UAVTALK_R150202_GPSPOSITIONSENSOR_OBJID, UAVTALK_R150202::handle_gpspositionsensor},
	{UAVTALK_R150202_GPSVELOCITYSENSOR_OBJID, UAVTALK_R150202::handle_gpsvelocitysensor},
	{UAVTALK_R150202_MANUALCONTROLCOMMAND_OBJID, UAVTALK_R150202::handle_manualcontrolcommand},
	{UAVTALK_R150202_POSITIONSTATE_OBJID, UAVTALK_R150202::handle_positionstate},
	{UAVTALK_R150202_SYSTEMSTATS_OBJID, UAVTALK_R150202::handle_systemstats},
	{_UT_TIMEOUT_OBJID, UAVTALK_R150202::update_connection}
};

}  // namespace r150502

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_150502_RELEASE_H_ */
