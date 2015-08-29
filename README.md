MultiOSD
========

Yet another MinimOSD firmware with configurable screens.

![Screenshot](res/screenshot.png)

**Warning: this software in early alpha stage!**

Capabilities:
   * switching between screens
   * configuration console
   * no image snow
   * ADC RSSI and battery stats - useful with simple FC, such as CC3D


Supported flight controllers
----------------------------

* All OpenPilot boards: CC, CC3D/CC3D ATOM/CC3D EVO (tested), CC3D REVO
* Many of MAVLink boards:
   * APM 2.x (tested)
   * Pixhawk PX4
   * Paparazzi
   * Generic MAVLink boards

*Testers are needed!*

Already available indicators
----------------------------

- Altitude
- Climb rate
- Flight mode
- Armed indicator
- Connected indicator
- Flight time (time since FC boot actually)
- Roll angle
- Pitch angle
- GPS state
- GPS Latitude
- GPS Longitude
- Artifical horizon
- Throttle
- Ground speed
- Battery voltage
- Battery current
- Battery consumed
- RSSI flag
- Distance to home
- Direction to home
- Callsign
- Temperature
- RSSI level
- Compass

More to come.

Known bugs and problems
-----------------------

- No configuration software!
- No documentation!
- Home direction bug
- CC3D Revolution home distance/direction bug
- Empty screen when cold started. Workaround: just press reset button on MinimOSD



