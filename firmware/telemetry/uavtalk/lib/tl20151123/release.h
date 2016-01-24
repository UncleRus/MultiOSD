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

#include "../op150202/release.h"

#include "../common.h"

#include "attitudeactual.h"
#include "baroaltitude.h"
#include "flightbatterystate.h"
#include "flightstatus.h"
#include "flighttelemetrystats.h"
#include "gcstelemetrystats.h"
#include "gpsposition.h"
#include "gpsvelocity.h"
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	#include "magnetometer.h"
#endif
#include "manualcontrolcommand.h"
#include "nedposition.h"
#include "systemstats.h"
#include "airspeedactual.h"

UT_NAMESPACE_OPEN

namespace tl20151123
{

#define UAVTALK_TL20151123 telemetry::modules::uavtalk::tl20151123

namespace fm
{
	const char acro [] PROGMEM = "ACRO";
	const char levl [] PROGMEM = "LEVL";
	const char mwrt [] PROGMEM = "MWRT";
	const char horz [] PROGMEM = "HORZ";
	const char axlk [] PROGMEM = "AXLK";
	const char virt [] PROGMEM = "VIRT";
	const char atun [] PROGMEM = "ATUN";
	const char alth [] PROGMEM = "ALTH";
	const char rth  [] PROGMEM = "RTH ";
	const char tblt [] PROGMEM = "TBLT";
	const char acrp [] PROGMEM = "ACR+";

	const char * const names [] PROGMEM = {
		UAVTALK_OP150202::fm::manu, acro, levl, mwrt, horz,
		axlk, virt, UAVTALK_OP150202::fm::stb1, UAVTALK_OP150202::fm::stb2, UAVTALK_OP150202::fm::stb3,
		atun, alth, UAVTALK_OP150202::fm::posh, rth, UAVTALK_OP150202::fm::plan,
		tblt, acrp
	};

	const uint8_t size = sizeof (names) / sizeof (char *);
}

void update_connection ();

#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate ();
#endif
void handle_flightstatus ();
void handle_flighttelemetrystats ();
void handle_gpsposition ();
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
void handle_magnetometer ();
#endif
void handle_manualcontrolcommand ();
void handle_airspeedactual ();

const obj_handler_t handlers [] PROGMEM = {
	{UAVTALK_TL20151123_FLIGHTSTATUS_OBJID,         handle_flightstatus},
	{UAVTALK_TL20151123_ATTITUDEACTUAL_OBJID,       UAVTALK_OP150202::handle_attitudestate},
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	{UAVTALK_TL20151123_BAROALTITUDE_OBJID,         UAVTALK_OP150202::handle_barosensor},
#endif
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
	{UAVTALK_TL20151123_FLIGHTBATTERYSTATE_OBJID,   handle_flightbatterystate},
#endif
	{UAVTALK_TL20151123_FLIGHTTELEMETRYSTATS_OBJID, handle_flighttelemetrystats},
	{UAVTALK_TL20151123_GPSPOSITION_OBJID,          handle_gpsposition},
	{UAVTALK_TL20151123_GPSVELOCITY_OBJID,          UAVTALK_OP150202::handle_gpsvelocitysensor},
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	{UAVTALK_TL20151123_MAGNETOMETER_OBJID,         handle_magnetometer},
#endif
	{UAVTALK_TL20151123_MANUALCONTROLCOMMAND_OBJID, handle_manualcontrolcommand},
	{UAVTALK_TL20151123_NEDPOSITION_OBJID,          UAVTALK_OP150202::handle_positionstate},
	{UAVTALK_TL20151123_SYSTEMSTATS_OBJID,          UAVTALK_OP150202::handle_systemstats},
	{UAVTALK_TL20151123_AIRSPEEDACTUAL_OBJID,       handle_airspeedactual},
	{_UT_TIMEOUT_OBJID,                             update_connection}
};

}  // namespace tl20151123

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_TL20151123_RELEASE_H_ */
