/*
 * This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
 *
 * MultiOSD is free software: you can redistribute it and/or modify
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
#ifndef TELEMETRY_UAVTALK_LIB_DR201702131_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_DR201702131_RELEASE_H_

#include "../common.h"
#include "../tl20151123/release.h"

#include "attitudeactual.h"
#include "flightbatterystate.h"
#include "flightstatus.h"
#include "systemstats.h"

UT_NAMESPACE_OPEN

namespace dr201702131
{

#define UAVTALK_DR201702131 telemetry::modules::uavtalk::dr201702131

const bool instid_required = false;

const uint32_t flightstatus_objid = UAVTALK_DR201702131_FLIGHTSTATUS_OBJID;

namespace fm
{
    const char acrd[] PROGMEM = "ACRD";
	const char fail[] PROGMEM = "FAIL";

	const char * const names[] PROGMEM = {
		UAVTALK_OP150202::fm::manu,   UAVTALK_TL20151123::fm::acro, UAVTALK_TL20151123::fm::lvl,
		UAVTALK_TL20151123::fm::hrzn, UAVTALK_TL20151123::fm::alck, UAVTALK_TL20151123::fm::vbar,
		UAVTALK_OP150202::fm::stb1,   UAVTALK_OP150202::fm::stb2,   UAVTALK_OP150202::fm::stb3,
		UAVTALK_TL20151123::fm::tune, UAVTALK_TL20151123::fm::ahld, UAVTALK_OP150202::fm::phld,
		UAVTALK_TL20151123::fm::rth,  UAVTALK_OP150202::fm::plan,   UAVTALK_TL20151123::fm::tab,
		UAVTALK_TL20151123::fm::acrp, acrd, fail
	};

	const uint8_t size = sizeof(names) / sizeof(char *);
}

#if !defined(TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate();
#endif

const obj_handler_t handlers[] PROGMEM = {
	{UAVTALK_DR201702131_FLIGHTSTATUS_OBJID,        UAVTALK_TL20151123::handle_flightstatus},
	{UAVTALK_DR201702131_ATTITUDEACTUAL_OBJID,      UAVTALK_OP150202::handle_attitudestate},
#if !defined(TELEMETRY_MODULES_I2C_BARO)
	{UAVTALK_TL20151123_BAROALTITUDE_OBJID,         UAVTALK_OP150202::handle_barosensor},
#endif
#if !defined(TELEMETRY_MODULES_ADC_BATTERY)
	{UAVTALK_DR201702131_FLIGHTBATTERYSTATE_OBJID,  UAVTALK_DR201702131::handle_flightbatterystate},
#endif
	{UAVTALK_TL20151123_FLIGHTTELEMETRYSTATS_OBJID, UAVTALK_TL20151123::handle_flighttelemetrystats},
	{UAVTALK_TL20151123_GPSPOSITION_OBJID,          UAVTALK_TL20151123::handle_gpsposition},
	{UAVTALK_TL20151123_GPSVELOCITY_OBJID,          UAVTALK_OP150202::handle_gpsvelocitysensor},
#if !defined(TELEMETRY_MODULES_I2C_COMPASS)
	{UAVTALK_TL20151123_MAGNETOMETER_OBJID,         UAVTALK_TL20151123::handle_magnetometer},
#endif
	{UAVTALK_TL20151123_MANUALCONTROLCOMMAND_OBJID, UAVTALK_TL20151123::handle_manualcontrolcommand},
	{UAVTALK_TL20151123_NEDPOSITION_OBJID,          UAVTALK_OP150202::handle_positionstate},
	{UAVTALK_DR201702131_SYSTEMSTATS_OBJID,         UAVTALK_OP150202::handle_systemstats},
	{UAVTALK_TL20151123_AIRSPEEDACTUAL_OBJID,       UAVTALK_TL20151123::handle_airspeedactual},
	{_UT_TIMEOUT_OBJID,                             UAVTALK_TL20151123::update_connection}
};

}  // namespace dr201702131

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_DR201702131_RELEASE_H_ */
