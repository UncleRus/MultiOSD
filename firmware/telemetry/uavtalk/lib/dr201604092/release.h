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
#ifndef TELEMETRY_UAVTALK_LIB_DR201604092_RELEASE_H_
#define TELEMETRY_UAVTALK_LIB_DR201604092_RELEASE_H_

#include "../common.h"
#include "../tl20151123/release.h"

#include "flightstatus.h"
#include "attitudeactual.h"
#include "systemstats.h"

UT_NAMESPACE_OPEN

namespace dr201604092
{

#define UAVTALK_DR201604092 telemetry::modules::uavtalk::dr201604092

namespace fm
{
	const char fsaf [] PROGMEM = "FSAF";

	const char * const names [] PROGMEM = {
		UAVTALK_OP150202::fm::manu, UAVTALK_TL20151123::fm::acro, UAVTALK_TL20151123::fm::levl,
		UAVTALK_TL20151123::fm::mwrt, UAVTALK_TL20151123::fm::horz, UAVTALK_TL20151123::fm::axlk,
		UAVTALK_TL20151123::fm::virt, UAVTALK_OP150202::fm::stb1, UAVTALK_OP150202::fm::stb2,
		UAVTALK_OP150202::fm::stb3, UAVTALK_TL20151123::fm::atun, UAVTALK_TL20151123::fm::alth,
		UAVTALK_OP150202::fm::posh, UAVTALK_TL20151123::fm::rth, UAVTALK_OP150202::fm::plan,
		UAVTALK_TL20151123::fm::tblt, UAVTALK_TL20151123::fm::acrp, fsaf
	};

	const uint8_t size = sizeof (names) / sizeof (char *);
}

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_DR201604092_FLIGHTSTATUS_OBJID,        UAVTALK_TL20151123::handle_flightstatus},
	{UAVTALK_DR201604092_ATTITUDEACTUAL_OBJID,      UAVTALK_OP150202::handle_attitudestate},
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	{UAVTALK_TL20151123_BAROALTITUDE_OBJID,         UAVTALK_OP150202::handle_barosensor},
#endif
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
	{UAVTALK_TL20151123_FLIGHTBATTERYSTATE_OBJID,   UAVTALK_TL20151123::handle_flightbatterystate},
#endif
	{UAVTALK_TL20151123_FLIGHTTELEMETRYSTATS_OBJID, UAVTALK_TL20151123::handle_flighttelemetrystats},
	{UAVTALK_TL20151123_GPSPOSITION_OBJID,          UAVTALK_TL20151123::handle_gpsposition},
	{UAVTALK_TL20151123_GPSVELOCITY_OBJID,          UAVTALK_OP150202::handle_gpsvelocitysensor},
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	{UAVTALK_TL20151123_MAGNETOMETER_OBJID,         UAVTALK_TL20151123::handle_magnetometer},
#endif
	{UAVTALK_TL20151123_MANUALCONTROLCOMMAND_OBJID, UAVTALK_TL20151123::handle_manualcontrolcommand},
	{UAVTALK_TL20151123_NEDPOSITION_OBJID,          UAVTALK_OP150202::handle_positionstate},
	{UAVTALK_DR201604092_SYSTEMSTATS_OBJID,         UAVTALK_OP150202::handle_systemstats},
	{UAVTALK_TL20151123_AIRSPEEDACTUAL_OBJID,       UAVTALK_TL20151123::handle_airspeedactual},
	{_UT_TIMEOUT_OBJID,                             UAVTALK_TL20151123::update_connection}
};

}  // namespace DR201604092

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_DR201604092_RELEASE_H_ */
