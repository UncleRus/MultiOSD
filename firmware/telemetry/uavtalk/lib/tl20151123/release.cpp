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
	bool was_armed = telemetry::status::armed;
	telemetry::status::armed = obj->Armed == FLIGHTSTATUS_ARMED_ARMED;
	telemetry::status::flight_mode = obj->FlightMode;
	telemetry::status::flight_mode_name_p = uavtalk::get_fm_name_p (telemetry::status::flight_mode);

	if (internal_home_calc && !was_armed && telemetry::status::armed)
		telemetry::home::fix ();
}

void handle_baroaltitude ()
{
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	BaroAltitude *obj = (BaroAltitude *) &buffer.data;
	telemetry::barometer::altitude = obj->Altitude;
	telemetry::barometer::pressure = obj->Pressure;
	telemetry::environment::temperature = telemetry::barometer::temperature = obj->Temperature;
	telemetry::stable::update_alt_climb (telemetry::barometer::altitude);
#endif
}

void handle_flightbatterystate ()
{
#if !defined (TELEMETRY_MODULES_ADC_BATTERY)
	FlightBatteryState *obj = (FlightBatteryState *) &buffer.data;
	telemetry::battery::voltage = obj->Voltage;
	telemetry::battery::update_voltage ();
	telemetry::battery::current = obj->Current;
	telemetry::battery::consumed = obj->ConsumedEnergy;
#endif
}

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
	telemetry::status::connection = fts_respond (obj->Status);
	connection_timeout = telemetry::ticks + UAVTALK_CONNECTION_TIMEOUT;
}

void handle_gpsposition ()
{
	UAVTALK_OP150202::handle_gpspositionsensor ();
	if (telemetry::gps::state > 0) telemetry::gps::state --;
}

void handle_gpsvelocity ()
{
	GPSVelocity *obj = (GPSVelocity *) &buffer.data;
	telemetry::gps::climb = -obj->Down;
}

void handle_manualcontrolcommand ()
{
	ManualControlCommand *obj = (ManualControlCommand *) &buffer.data;
	telemetry::input::throttle   = (int8_t) (obj->Throttle * 100);
	telemetry::input::roll       = (int8_t) (obj->Roll * 100);
	telemetry::input::pitch      = (int8_t) (obj->Pitch * 100);
	telemetry::input::yaw        = (int8_t) (obj->Yaw * 100);
	telemetry::input::collective = (int8_t) (obj->Collective * 100);
	memcpy (telemetry::input::channels, obj->Channel, sizeof (obj->Channel));
	telemetry::input::connected = obj->Connected;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	telemetry::messages::rssi_low = !telemetry::input::connected;
	telemetry::input::rssi = obj->Rssi < 0 ? 0 : (obj->Rssi > 100 ? 100 : obj->Rssi);
#endif
}

void handle_airspeedactual ()
{
	AirspeedActual *obj = (AirspeedActual *) &buffer.data;
	telemetry::stable::airspeed = obj->CalibratedAirspeed;
}

void update_connection ()
{
	send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
}

}  // namespace tl20151123

UT_NAMESPACE_CLOSE
