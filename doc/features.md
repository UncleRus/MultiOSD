# MultiOSD feature matrix

 Indicator            | UAVTalk      | MAVLink      | UBX     | MSP
----------------------|--------------|--------------|---------|---------------
 Altitude             | + (Baro/GPS) | + (Baro/GPS) | + (GPS) | + (FC/GPS)
 Climb rate (Vario)   | + (Baro/GPS) | + (Baro/GPS) | + (GPS) | + (FC/GPS)
 Flight mode          | +            | + [1]        |         | 
 Armed indicator      | +            | +            |         | +
 Connection state     | +            | +            | +       | +
 Flight time          | +            | +            | +       | +
 Roll                 | +            | +            |         | +
 Pitch                | +            | +            |         | +
 GPS state            | +            | +            | +       | + [2]
 GPS Latitude         | +            | +            | +       | +
 GPS Longitude        | +            | +            | +       | +
 Artifical horizon    | +            | +            |         | +
 Throttle             | +            | +            |         | 
 Ground speed (GPS)   | +            | +            | +       | + 
 Batt. 1 Voltage      | +            | +            |         | +
 Batt. 1 Amperage     | +            | +            |         | +
 Batt. 1 Consumed     | +            | +            |         | +
 Batt. 2 Voltage [3]  |              |              |         |
 Batt. 2 Amperage [3] |              |              |         |
 Batt. 2 Consumed [3] |              |              |         |
 RSSI low indicator   | + [4]        | + [5]        |         | +
 Home distance        | + (FC/GPS)   | + (GPS)      | + (GPS) | + (FC/GPS)
 Home direction       | + (FC/GPS)   | + (GPS)      | + (GPS) | + (FC/GPS)
 Callsign             | +            | +            | +       | +
 Environment temp.    | + (Baro)     | + (Baro)     |         |
 RSSI level           | + [4]        | + [5]        |         |
 Compass              | + (Mag/GPS)  | + (Mag/GPS)  | + (GPS) | + (FC)
 Airspeed             |              | +            |         |
 
 
- [1] - Fully supported ArduPilot/ArduCopter/Paparazzi, others partially
- [2] - Partially supported
- [3] - Battery 2 supported by ADCBattery module only
- [4] - Fully supported TauLabs, dRonin. For others RSSI value emulated
  based on connection state between receiver and flight controller 
- [5] - RSSI value can be emulated based on input values

