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
#include "release.h"

#include "stddef.h"
#include <string.h>
#include <math.h>
#include "../../../telemetry.h"

UT_NAMESPACE_OPEN

namespace op150202
{

void handle_flightstatus ()
{
	FlightStatus *obj = (FlightStatus *) &buffer.data;
	bool was_armed = status::armed;
	status::armed = obj->Armed == FLIGHTSTATUS_ARMED_ARMED;
	status::flight_mode = obj->FlightMode;
	status::flight_mode_name_p = uavtalk::get_fm_name_p (status::flight_mode);
	if (internal_home_calc && !was_armed && status::armed)
		home::fix ();
}

void handle_attitudestate ()
{
	AttitudeState *obj = (AttitudeState *) &buffer.data;
	attitude::roll  = obj->Roll;
	attitude::pitch = obj->Pitch;
	attitude::yaw   = obj->Yaw;
}

#if !defined (TELEMETRY_MODULES_I2C_BARO)
void handle_barosensor ()
{
	BaroSensor *obj = (BaroSensor *) &buffer.data;
	barometer::altitude = obj->Altitude;
	barometer::pressure = obj->Pressure;
	environment::temperature = barometer::temperature = obj->Temperature;
	stable::update_alt_climb (obj->Altitude);
	baro_enabled = true;
}
#endif

#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate ()
{
	FlightBatteryState *obj = (FlightBatteryState *) &buffer.data;
	battery::voltage = obj->Voltage;
	battery::cells = obj->NbCells;
	battery::update_voltage ();
	battery::current = obj->Current;
	battery::consumed = obj->ConsumedEnergy;
}
#endif

void send_gcs_telemetry_stats (GCSTelemetryStatsStatus status)
{
	header_t h;
	GCSTelemetryStats data;
	data.Status = status;
	h.msg_type = _UT_TYPE_OBJ_ACK;
	h.length = UAVTALK_HEADER_LEN + sizeof (GCSTelemetryStats);
	h.objid = UAVTALK_OP150202_GCSTELEMETRYSTATS_OBJID;
	send (h, (uint8_t *) &data, sizeof (GCSTelemetryStats));
}

inline uint8_t fts_respond (uint8_t state)
{
	if (state == FLIGHTTELEMETRYSTATS_STATUS_DISCONNECTED)
	{
		send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_HANDSHAKEREQ);
		return CONNECTION_STATE_ESTABLISHING;
	}

	if (state == FLIGHTTELEMETRYSTATS_STATUS_HANDSHAKEACK)
	{
		send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
		request_object (fts_objid);
	}

	return CONNECTION_STATE_CONNECTED;
}

void handle_flighttelemetrystats ()
{
	FlightTelemetryStats *obj = (FlightTelemetryStats *) &buffer.data;
	status::connection = fts_respond (obj->Status);
	connection_timeout = ticks + UAVTALK_CONNECTION_TIMEOUT;
}

void handle_gpspositionsensor ()
{
	GPSPositionSensor *obj = (GPSPositionSensor *) &buffer.data;
	gps::latitude   = obj->Latitude / 10000000.0;
	gps::longitude  = obj->Longitude / 10000000.0;
	gps::altitude   = obj->Altitude;
	gps::heading    = round (obj->Heading);
	stable::groundspeed = gps::speed = obj->Groundspeed;
	gps::state      = obj->Status;
	gps::satellites = obj->Satellites;
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	if (stable::heading_source == stable::hs_disabled
		|| stable::heading_source == stable::hs_gps)
	{
		stable::heading = gps::heading;
		stable::heading_source = stable::hs_gps;
	}
#endif
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	// update stable altitude if we can't get the baro altitude
	if (!baro_enabled) stable::update_alt_climb (gps::altitude);
#endif
	// calc home distance/direction based on gps
	if (internal_home_calc) home::update ();
}

void handle_gpsvelocitysensor ()
{
	GPSVelocitySensor *obj = (GPSVelocitySensor *) &buffer.data;
	gps::climb = -obj->Down;
}

void handle_manualcontrolcommand ()
{
	ManualControlCommand *obj = (ManualControlCommand *) &buffer.data;
	input::throttle   = (int8_t) (obj->Throttle * 100);
	input::roll       = (int8_t) (obj->Roll * 100);
	input::pitch      = (int8_t) (obj->Pitch * 100);
	input::yaw        = (int8_t) (obj->Yaw * 100);
	input::collective = (int8_t) (obj->Collective * 100);
	input::thrust     = (int8_t) (obj->Thrust * 100);
	memcpy (input::channels, obj->Channel, sizeof (obj->Channel));
	input::connected = obj->Connected;
	input::flight_mode_switch = obj->FlightModeSwitchPosition;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	messages::rssi_low = !input::connected;
	input::rssi = input::connected ? 100 : 0;
#endif
}

void handle_positionstate ()
{
	if (internal_home_calc) return;

	home::state = HOME_STATE_FIXED;

	PositionState *obj = (PositionState *) &buffer.data;
	home::distance = sqrt (square (obj->East) + square (obj->North));
	int16_t bearing = atan2 (obj->East, obj->North) * 57.295775;
	if (bearing < 0) bearing += 360;
	if (bearing > 360) bearing -= 360;
	home::direction = bearing;
}

#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
void handle_magsensor ()
{
	MagSensor *obj = (MagSensor *) &buffer.data;
	if (mag_enabled) return;

	stable::heading_source = stable::hs_internal_mag;
	stable::calc_heading (obj->x, obj->y);
}

void handle_magstate ()
{
	MagState *obj = (MagState *) &buffer.data;
	if (obj->Source == MAGSTATE_SOURCE_INVALID) return;

	stable::heading_source = obj->Source == MAGSTATE_SOURCE_ONBOARD
		? stable::hs_internal_mag
		: stable::hs_external_mag;
	stable::calc_heading (obj->x, obj->y);
	mag_enabled = true;
}
#endif

void handle_systemstats ()
{
	SystemStats *obj = (SystemStats *) &buffer.data;
	status::flight_time = obj->FlightTime / 1000;
}

void update_connection ()
{
	send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
}

}  // namespace op150202

UT_NAMESPACE_CLOSE
