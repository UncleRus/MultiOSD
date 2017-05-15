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

void handle_flightstatus()
{
	FlightStatus *obj = (FlightStatus *)&buffer.data;
	bool was_armed = status::armed;
	status::armed = obj->Armed == FLIGHTSTATUS_ARMED_ARMED;
	status::flight_mode = obj->FlightMode;
	status::flight_mode_name_p = uavtalk::get_fm_name_p(status::flight_mode);

	if (internal_home_calc && !was_armed && status::armed)
		home::fix();
}

#if !defined(TELEMETRY_MODULES_ADC_BATTERY)
void handle_flightbatterystate()
{
	FlightBatteryState *obj = (FlightBatteryState *)&buffer.data;
	battery::battery1.set_voltage(obj->Voltage, true);
	battery::battery1.amperage = obj->Current;
	battery::battery1.consumed = obj->ConsumedEnergy;
}
#endif

void send_gcs_telemetry_stats(GCSTelemetryStatsStatus status)
{
	header_t h(_UT_TYPE_OBJ_ACK, header_len + sizeof(GCSTelemetryStats), UAVTALK_TL20151123_GCSTELEMETRYSTATS_OBJID);
	GCSTelemetryStats data;
	data.Status = status;
	send(h, (uint8_t *)&data, sizeof(GCSTelemetryStats));
}

inline status::connection_state_t fts_respond(uint8_t state)
{
	if (state == FLIGHTTELEMETRYSTATS_STATUS_DISCONNECTED)
	{
		send_gcs_telemetry_stats(GCSTELEMETRYSTATS_STATUS_HANDSHAKEREQ);
		return status::ESTABLISHING;
	}

	if (state == FLIGHTTELEMETRYSTATS_STATUS_HANDSHAKEACK)
		send_gcs_telemetry_stats(GCSTELEMETRYSTATS_STATUS_CONNECTED);

	request_object(release.flightstatus_objid);
	return status::CONNECTED;
}

void handle_flighttelemetrystats()
{
    FlightTelemetryStats *obj = (FlightTelemetryStats *)&buffer.data;
    status::connection = fts_respond(obj->Status);
    connection_timeout = update_time;
}

void handle_gpsposition()
{
    GPSPosition *obj = (GPSPosition *)&buffer.data;
    gps::latitude   = obj->Latitude / 10000000.0;
    gps::longitude  = obj->Longitude / 10000000.0;
    gps::altitude   = obj->Altitude;
    gps::heading    = round(obj->Heading);
    stable::groundspeed = gps::speed = obj->Groundspeed;
    gps::satellites = obj->Satellites;
    gps::pdop       = obj->PDOP;
    gps::hdop       = obj->HDOP;
    gps::vdop       = obj->VDOP;
    switch (obj->Status)
    {
        case GPSPOSITION_STATUS_FIX2D:
            gps::state = GPS_STATE_2D;
            break;
        case GPSPOSITION_STATUS_FIX3D:
        case GPSPOSITION_STATUS_DIFF3D:
            gps::state = GPS_STATE_3D;
            break;
        default:
            gps::state = GPS_STATE_NO_FIX;
            break;
    }
    gps::update(internal_home_calc, !baro_enabled);
}

void handle_gpsvelocity()
{
	GPSVelocity *obj = (GPSVelocity *)&buffer.data;
	gps::climb = -obj->Down;
}

#if !defined(TELEMETRY_MODULES_I2C_COMPASS)
void handle_magnetometer()
{
    if (use_attituide_heading) return;

	Magnetometer *obj = (Magnetometer *)&buffer.data;
	stable::heading_source = stable::INTERNAL_MAG;
	stable::calc_heading(obj->x, obj->y);
	mag_enabled = true;
	if (internal_home_calc)
	    home::update();
}
#endif

void handle_manualcontrolcommand()
{
	ManualControlCommand *obj = (ManualControlCommand *)&buffer.data;
	input::throttle   = (int8_t)(obj->Throttle * 100);
	input::roll       = (int8_t)(obj->Roll * 100);
	input::pitch      = (int8_t)(obj->Pitch * 100);
	input::yaw        = (int8_t)(obj->Yaw * 100);
	memcpy(input::channels, obj->Channel, sizeof(obj->Channel));
	input::connected = obj->Connected;
#if !defined (TELEMETRY_MODULES_ADC_RSSI)
	input::set_rssi(!emulate_rssi ? obj->Rssi : (input::connected ? 100 : 0));
#endif
}

void handle_airspeedactual()
{
	AirspeedActual *obj = (AirspeedActual *)&buffer.data;
	stable::airspeed = obj->CalibratedAirspeed;
}

void update_connection()
{
	send_gcs_telemetry_stats(GCSTELEMETRYSTATS_STATUS_CONNECTED);
}

}  // namespace tl20151123

UT_NAMESPACE_CLOSE
