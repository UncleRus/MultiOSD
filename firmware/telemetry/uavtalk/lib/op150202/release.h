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
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	#include "magsensor.h"
	#include "magstate.h"
#endif
#include "manualcontrolcommand.h"
#include "positionstate.h"
#include "systemstats.h"

UT_NAMESPACE_OPEN

namespace op150202
{

#define UAVTALK_OP150202 telemetry::modules::uavtalk::op150202

const bool instid_required = true;

const uint32_t flightstatus_objid = UAVTALK_OP150202_FLIGHTSTATUS_OBJID;

void update_connection();

void handle_attitudestate();
#if !defined(TELEMETRY_MODULES_I2C_BARO)
void handle_barosensor();
#endif
#if !defined(TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate();
#endif
void handle_flightstatus();
void handle_flighttelemetrystats();
void handle_gpspositionsensor();
void handle_gpsvelocitysensor();
void handle_manualcontrolcommand();
void handle_positionstate();
void handle_magsensor();
void handle_magstate();
void handle_systemstats();

namespace fm
{

	const char manu[] PROGMEM = "MANU";  // Manual
	const char stb1[] PROGMEM = "STB1";  // Stabilized 1
	const char stb2[] PROGMEM = "STB2";  // Stabilized 2
	const char stb3[] PROGMEM = "STB3";  // Stabilized 3
	const char stb4[] PROGMEM = "STB4";  // Stabilized 4
	const char stb5[] PROGMEM = "STB5";  // Stabilized 5
	const char stb6[] PROGMEM = "STB6";  // Stabilized 6
	const char phld[] PROGMEM = "PHLD";  // Position Hold
	const char clck[] PROGMEM = "CLCK";  // Course lock
	const char roam[] PROGMEM = "ROAM";  // Position Roam
	const char hlea[] PROGMEM = "HLEA";  // Home Leash
	const char absp[] PROGMEM = "ABSP";  // Absolute Position
	const char rtb [] PROGMEM = "RTB ";  // Return to Base
	const char land[] PROGMEM = "LAND";  // Land
	const char plan[] PROGMEM = "PLAN";  // Path Planner
	const char poi [] PROGMEM = "POI ";  // Point of Interest
	const char acru[] PROGMEM = "ACRU";  // Aurocruise

	const char * const names[] PROGMEM = {
		manu, stb1, stb2, stb3, stb4,
		stb5, phld, clck, roam, hlea,
		absp, rtb,  land, plan, poi,
		acru
	};

	const uint8_t size = sizeof(names) / sizeof(char *);

}  // namespace fm

const obj_handler_t handlers[] PROGMEM = {
	{UAVTALK_OP150202_FLIGHTSTATUS_OBJID,         handle_flightstatus},
	{UAVTALK_OP150202_ATTITUDESTATE_OBJID,        handle_attitudestate},
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	{UAVTALK_OP150202_BAROSENSOR_OBJID,           handle_barosensor},
#endif
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
	{UAVTALK_OP150202_FLIGHTBATTERYSTATE_OBJID,   handle_flightbatterystate},
#endif
	{UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID, handle_flighttelemetrystats},
	{UAVTALK_OP150202_GPSPOSITIONSENSOR_OBJID,    handle_gpspositionsensor},
	{UAVTALK_OP150202_GPSVELOCITYSENSOR_OBJID,    handle_gpsvelocitysensor},
	{UAVTALK_OP150202_MANUALCONTROLCOMMAND_OBJID, handle_manualcontrolcommand},
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	{UAVTALK_OP150202_MAGSENSOR_OBJID,            handle_magsensor},
	{UAVTALK_OP150202_MAGSTATE_OBJID,             handle_magstate},
#endif
	{UAVTALK_OP150202_POSITIONSTATE_OBJID,        handle_positionstate},
	{UAVTALK_OP150202_FLIGHTTELEMETRYSTATS_OBJID, handle_flighttelemetrystats},
	{UAVTALK_OP150202_SYSTEMSTATS_OBJID,          handle_systemstats},
	{_UT_TIMEOUT_OBJID,                           update_connection}
};

}  // namespace op150202

UT_NAMESPACE_CLOSE

#endif /* TELEMETRY_UAVTALK_LIB_OP150202_RELEASE_H_ */
