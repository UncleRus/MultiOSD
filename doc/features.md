# OSD feature matrix

 Indicator           | UAVTalk      | MAVLink      | UBX     | MSP
---------------------|--------------|--------------|---------|---------------
 Altitude            | + (Baro/GPS) | + (Baro/GPS) | + (GPS) | + (Baro/GPS)
 Climb rate (Vario)  | + (Baro/GPS) | + (Baro/GPS) | + (GPS) | + (Baro/GPS)
 Flight mode         | +            | + [1]        |         | + [2]
 Armed indicator     | +            | +            |         | +
 Connection state    | +            | +            | +       | +
 Flight time         | +            | +            | +       | +
 Roll                | +            | +            |         | +
 Pitch               | +            | +            |         | +
 GPS state           | +            | +            | +       | + [3]
 GPS Latitude        | +            | +            | +       | +
 GPS Longitude       | +            | +            | +       | +
 Artifical horizon   | +            | +            |         | +
 Throttle            | +            | +            |         | 
 Ground speed (GPS)  | +            | +            | +       | + 
 Batt. 1 Voltage     | +            | +            |         | +
 Batt. 2 Voltage [4] |              |              |         |
 Batt. 1 Amperage    | +            | +            |         | +
 Batt. 1 Consumed    | +            | +            |         | +
 RSSI low indicator  | + [5]        | + [6]        |         | + [6]
 Home distance       | + (FC/GPS)   | + (GPS)      | + (GPS) | + (GPS)
 Home direction      | + (FC/GPS)   | + (GPS)      | + (GPS) | + (GPS)
 Callsign            | +            | +            | +       | +
 Environment temp.   | + (Baro)     | + (Baro)     |         |
 RSSI level          | + [5]        | + [6]        |         |
 Compass             | + (Mag/GPS)  | + (Mag/GPS)  | + (GPS) | + (FC)
 Airspeed            |              | +            |         |
 
 
- [1] - Fully supported ArduPilot/ArduCopter/Paparazzi, others partially
- [2] - Partially supported dRonin and CleanFlight, others not supported
- [3] - Partially supported
- [4] - Battery 2 Voltage supported by ADCBattery module only
- [5] - RSSI value emulated based on connection state between receiver and flight controller 
- [6] - RSSI value emulated based on input values
