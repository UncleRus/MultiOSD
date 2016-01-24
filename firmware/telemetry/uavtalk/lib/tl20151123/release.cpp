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

namespace tl20151123
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

#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate ()
{
	FlightBatteryState *obj = (FlightBatteryState *) &buffer.data;
	battery::voltage = obj->Voltage;
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
	h.objid = UAVTALK_TL20151123_GCSTELEMETRYSTATS_OBJID;
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

void handle_gpsposition ()
{
	UAVTALK_OP150202::handle_gpspositionsensor ();
	if (gps::state > 0) gps::state --;
}

void handle_gpsvelocity ()
{
	GPSVelocity *obj = (GPSVelocity *) &buffer.data;
	gps::climb = -obj->Down;
}

#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
void handle_magnetometer ()
{
	Magnetometer *obj = (Magnetometer *) &buffer.data;
	stable::heading_source = stable::hs_external_mag;
	stable::calc_heading (obj->x, obj->y);
	mag_enabled = true;
}
#endif

void handle_manualcontrolcommand ()
{
	ManualControlCommand *obj = (ManualControlCommand *) &buffer.data;
	input::throttle   = (int8_t) (obj->Throttle * 100);
	input::roll       = (int8_t) (obj->Roll * 100);
	input::pitch      = (int8_t) (obj->Pitch * 100);
	input::yaw        = (int8_t) (obj->Yaw * 100);
	input::collective = (int8_t) (obj->Collective * 100);
	memcpy (input::channels, obj->Channel, sizeof (obj->Channel));
	input::connected = obj->Connected;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	input::rssi = obj->Rssi * 100;
	messages::rssi_low = input::rssi < rssi_low_threshold;
#endif
}

void handle_airspeedactual ()
{
	AirspeedActual *obj = (AirspeedActual *) &buffer.data;
	stable::airspeed = obj->CalibratedAirspeed;
}

void update_connection ()
{
	send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
}

}  // namespace tl20151123

UT_NAMESPACE_CLOSE
