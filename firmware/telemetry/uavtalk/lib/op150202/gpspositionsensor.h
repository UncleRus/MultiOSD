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
#ifndef UAVTALK_150202_GPSPOSITIONSENSOR_H
#define UAVTALK_150202_GPSPOSITIONSENSOR_H

#include "../common.h"

UT_NAMESPACE_OPEN

namespace op150202
{

#define UAVTALK_OP150202_GPSPOSITIONSENSOR_OBJID 0x7D26A6E6

enum GPSPositionSensorStatus
{
    GPSPOSITIONSENSOR_STATUS_NOGPS = 0,
    GPSPOSITIONSENSOR_STATUS_NOFIX = 1,
    GPSPOSITIONSENSOR_STATUS_FIX2D = 2,
    GPSPOSITIONSENSOR_STATUS_FIX3D = 3
};

enum GPSPositionSensorSensorType
{
    GPSPOSITIONSENSOR_SENSORTYPE_UNKNOWN = 0,
    GPSPOSITIONSENSOR_SENSORTYPE_NMEA = 1,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX = 2,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX7 = 3,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX8 = 4
};

enum GPSPositionSensorAutoConfigStatus
{
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_DISABLED = 0,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_RUNNING = 1,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_DONE = 2,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_ERROR = 3
};

struct GPSPositionSensor
{
    int32_t Latitude;
    int32_t Longitude;
    float Altitude;
    float GeoidSeparation;
    float Heading;
    float Groundspeed;
    float PDOP;
    float HDOP;
    float VDOP;
    GPSPositionSensorStatus Status;
    int8_t Satellites;
    GPSPositionSensorSensorType SensorType;
    GPSPositionSensorAutoConfigStatus AutoConfigStatus;
};

}  // namespace op150202

UT_NAMESPACE_CLOSE

#endif // UAVTALK_150202_GPSPOSITIONSENSOR_H
