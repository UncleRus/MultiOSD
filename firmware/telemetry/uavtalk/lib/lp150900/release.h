/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 *(at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef TELEMETRY_UAVTALK_LIB_LP150900_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_LP150900_RELEASE_H_

#include "../op150202/release.h"
#include "../op150502/release.h"

#include "../common.h"

#include "gpspositionsensor.h"
#include "manualcontrolcommand.h"

UT_NAMESPACE_OPEN

namespace lp150900
{

#define UAVTALK_LP150900 telemetry::modules::uavtalk::lp150900

const bool instid_required = true;

const uint32_t flightstatus_objid = UAVTALK_OP150502_FLIGHTSTATUS_OBJID;

void handle_manualcontrolcommand();

const obj_handler_t handlers [] PROGMEM = {
    {UAVTALK_OP150502_FLIGHTSTATUS_OBJID,         UAVTALK_OP150202::handle_flightstatus},
    {UAVTALK_OP150202_ATTITUDESTATE_OBJID,        UAVTALK_OP150202::handle_attitudestate},
#if !defined(TELEMETRY_MODULES_I2C_BARO)
    {UAVTALK_OP150202_BAROSENSOR_OBJID,           UAVTALK_OP150202::handle_barosensor},
#endif
#if !defined(TELEMETRY_MODULES_ADC_BATTERY)
    {UAVTALK_OP150202_FLIGHTBATTERYSTATE_OBJID,   UAVTALK_OP150202::handle_flightbatterystate},
#endif
    {UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID, UAVTALK_OP150202::handle_flighttelemetrystats},
    {UAVTALK_LP150900_GPSPOSITIONSENSOR_OBJID,    UAVTALK_OP150202::handle_gpspositionsensor}, // new obj_id, structs are pretty the same
    {UAVTALK_OP150202_GPSVELOCITYSENSOR_OBJID,    UAVTALK_OP150202::handle_gpsvelocitysensor},
    {UAVTALK_LP150900_MANUALCONTROLCOMMAND_OBJID, UAVTALK_LP150900::handle_manualcontrolcommand}, // new struct
#if !defined(TELEMETRY_MODULES_I2C_COMPASS)
    {UAVTALK_OP150202_MAGSENSOR_OBJID,            UAVTALK_OP150202::handle_magsensor},
    {UAVTALK_OP150202_MAGSTATE_OBJID,             UAVTALK_OP150202::handle_magstate},
#endif
    {UAVTALK_OP150202_POSITIONSTATE_OBJID,        UAVTALK_OP150202::handle_positionstate},
    {UAVTALK_OP150202_SYSTEMSTATS_OBJID,          UAVTALK_OP150202::handle_systemstats},
    {_UT_TIMEOUT_OBJID,                           UAVTALK_OP150202::update_connection}
};

}  // namespace lp150900

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_LP150900_RELEASE_H_ */
