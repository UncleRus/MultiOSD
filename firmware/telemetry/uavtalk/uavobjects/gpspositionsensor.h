/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef UAVTALK_GPSPOSITIONSENSOR_H
#define UAVTALK_GPSPOSITIONSENSOR_H

#define UAVTALK_GPSPOSITIONSENSOR_OBJID 0x7D26A6E6
#define UAVTALK_GPSPOSITIONSENSOR_NUMBYTES sizeof (GPSPositionSensorData)

enum GPSPositionSensorStatusOptions
{
    GPSPOSITIONSENSOR_STATUS_NOGPS = 0,
    GPSPOSITIONSENSOR_STATUS_NOFIX = 1,
    GPSPOSITIONSENSOR_STATUS_FIX2D = 2,
    GPSPOSITIONSENSOR_STATUS_FIX3D = 3
};

enum GPSPositionSensorSensorTypeOptions
{
    GPSPOSITIONSENSOR_SENSORTYPE_UNKNOWN = 0,
    GPSPOSITIONSENSOR_SENSORTYPE_NMEA = 1,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX = 2,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX7 = 3,
    GPSPOSITIONSENSOR_SENSORTYPE_UBX8 = 4
};

enum GPSPositionSensorAutoConfigStatusOptions
{
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_DISABLED = 0,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_RUNNING = 1,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_DONE = 2,
    GPSPOSITIONSENSOR_AUTOCONFIGSTATUS_ERROR = 3
};

struct GPSPositionSensorData
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
    GPSPositionSensorStatusOptions Status;
    int8_t Satellites;
    GPSPositionSensorSensorTypeOptions SensorType;
    GPSPositionSensorAutoConfigStatusOptions AutoConfigStatus;
};

#endif // UAVTALK_GPSPOSITIONSENSOR_H
