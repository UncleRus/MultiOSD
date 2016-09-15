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
#ifndef TELEMETRY_UAVTALK_LIB_LP1609RC1_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_LP1609RC1_RELEASE_H_

#include "../op150202/release.h"
#include "../op150502/release.h"
#include "../lp150900/release.h"

#include "../common.h"

#include "flightstatus.h"
#include "gpspositionsensor.h"
#include "systemstats.h"

UT_NAMESPACE_OPEN

namespace lp1609rc1
{

#define UAVTALK_LP1609RC1 telemetry::modules::uavtalk::lp1609rc1

const bool instid_required = true;

const uint32_t flightstatus_objid = UAVTALK_LP1609RC1_FLIGHTSTATUS_OBJID;

namespace fm
{
	const char tune [] PROGMEM = "TUNE";

	const char * const names [] PROGMEM = {
		UAVTALK_OP150202::fm::manu, UAVTALK_OP150202::fm::stb1, UAVTALK_OP150202::fm::stb2, UAVTALK_OP150202::fm::stb3,
		UAVTALK_OP150202::fm::stb4, UAVTALK_OP150202::fm::stb5, UAVTALK_OP150202::fm::stb6, UAVTALK_OP150202::fm::phld,
		UAVTALK_OP150202::fm::clck, UAVTALK_OP150202::fm::roam, UAVTALK_OP150202::fm::hlea, UAVTALK_OP150202::fm::absp,
		UAVTALK_OP150202::fm::rtb,  UAVTALK_OP150202::fm::land, UAVTALK_OP150202::fm::plan, UAVTALK_OP150202::fm::poi,
		UAVTALK_OP150202::fm::acru, UAVTALK_OP150502::fm::atof, tune
	};

	const uint8_t size = sizeof (names) / sizeof (char *);
}

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_LP1609RC1_FLIGHTSTATUS_OBJID,        UAVTALK_OP150202::handle_flightstatus},         // new obj_id
	{UAVTALK_OP150202_ATTITUDESTATE_OBJID,        UAVTALK_OP150202::handle_attitudestate},
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	{UAVTALK_OP150202_BAROSENSOR_OBJID,           UAVTALK_OP150202::handle_barosensor},
#endif
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
	{UAVTALK_OP150202_FLIGHTBATTERYSTATE_OBJID,   UAVTALK_OP150202::handle_flightbatterystate},
#endif
	{UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID, UAVTALK_OP150202::handle_flighttelemetrystats},
	{UAVTALK_LP1609RC1_GPSPOSITIONSENSOR_OBJID,   UAVTALK_OP150202::handle_gpspositionsensor},    // new obj_id
	{UAVTALK_OP150202_GPSVELOCITYSENSOR_OBJID,    UAVTALK_OP150202::handle_gpsvelocitysensor},
	{UAVTALK_LP150900_MANUALCONTROLCOMMAND_OBJID, UAVTALK_LP150900::handle_manualcontrolcommand},
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	{UAVTALK_OP150202_MAGSENSOR_OBJID,            UAVTALK_OP150202::handle_magsensor},
	{UAVTALK_OP150202_MAGSTATE_OBJID,             UAVTALK_OP150202::handle_magstate},
#endif
	{UAVTALK_OP150202_POSITIONSTATE_OBJID,        UAVTALK_OP150202::handle_positionstate},
	{UAVTALK_LP1609RC1_SYSTEMSTATS_OBJID,         UAVTALK_OP150202::handle_systemstats},          // new obj_id
	{_UT_TIMEOUT_OBJID,                           UAVTALK_OP150202::update_connection}
};

}  // namespace lp1609rc1

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_LP1609RC1_RELEASE_H_ */
