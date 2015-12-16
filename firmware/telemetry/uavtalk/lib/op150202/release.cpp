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
	bool was_armed = telemetry::status::armed;
	telemetry::status::armed = obj->Armed == FLIGHTSTATUS_ARMED_ARMED;
	telemetry::status::flight_mode = obj->FlightMode;
	telemetry::status::flight_mode_name_p = telemetry::status::flight_mode < sizeof (fm::names) / sizeof (char *)
		? (const char *) pgm_read_ptr (&fm::names [telemetry::status::flight_mode])
		: NULL;
	// fix home if armed on CC3D
	if ((board == UAVTALK_BOARD_CC3D || internal_home_calc) && !was_armed && telemetry::status::armed)
		telemetry::home::fix ();
}

void handle_attitudestate ()
{
	AttitudeState *obj = (AttitudeState *) &buffer.data;
	telemetry::attitude::roll  = obj->Roll;
	telemetry::attitude::pitch = obj->Pitch;
	telemetry::attitude::yaw   = obj->Yaw;
}

void handle_barosensor ()
{
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	BaroSensor *obj = (BaroSensor *) &buffer.data;
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
	telemetry::battery::cells = obj->NbCells;
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
	telemetry::status::connection = fts_respond (obj->Status);
	connection_timeout = telemetry::ticks + UAVTALK_CONNECTION_TIMEOUT;
}

void handle_gpspositionsensor ()
{
	GPSPositionSensor *obj = (GPSPositionSensor *) &buffer.data;
	telemetry::gps::latitude    = obj->Latitude / 10000000.0;
	telemetry::gps::longitude   = obj->Longitude / 10000000.0;
	telemetry::gps::altitude    = obj->Altitude;
	telemetry::gps::heading     = round (obj->Heading);
	telemetry::stable::ground_speed = telemetry::gps::speed = obj->Groundspeed;
	telemetry::gps::state 		= obj->Status;
	telemetry::gps::satellites 	= obj->Satellites;
#if !defined (TELEMETRY_MODULES_I2C_COMPASS)
	// let's set heading if we don't have mag
	if (board == UAVTALK_BOARD_CC3D) telemetry::stable::heading = telemetry::gps::heading;
#endif
#if !defined (TELEMETRY_MODULES_I2C_BARO)
	// update stable altitude if we can't get the baro altitude
	if (board == UAVTALK_BOARD_CC3D) telemetry::stable::update_alt_climb (telemetry::gps::altitude);
#endif
	// calc home distance/direction based on gps
	if (board == UAVTALK_BOARD_CC3D || internal_home_calc) telemetry::home::update ();
}

void handle_gpsvelocitysensor ()
{
	GPSVelocitySensor *obj = (GPSVelocitySensor *) &buffer.data;
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
	telemetry::input::thrust     = (int8_t) (obj->Thrust * 100);
	memcpy (telemetry::input::channels, obj->Channel, sizeof (obj->Channel));
	telemetry::input::connected = obj->Connected;
	telemetry::input::flight_mode_switch = obj->FlightModeSwitchPosition;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	telemetry::messages::rssi_low = !telemetry::input::connected;
	telemetry::input::rssi = telemetry::input::connected ? 100 : 0;
#endif
}

void handle_positionstate ()
{
	// TODO : Revo home position, waypoints
}

void handle_systemstats ()
{
	SystemStats *obj = (SystemStats *) &buffer.data;
	telemetry::status::flight_time = obj->FlightTime / 1000;
}

void update_connection ()
{
	send_gcs_telemetry_stats (GCSTELEMETRYSTATS_STATUS_CONNECTED);
}

}  // namespace op150202

UT_NAMESPACE_CLOSE
