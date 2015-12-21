# MultiOSD

Yet another MinimOSD firmware with configurable screens layout.

![Screenshot](doc/source/img/screenshot.png)

* switching between screens
* configuration console
* no image noise
* ADC RSSI and battery stats

## Supported flight controllers and software

### UAVTalk module

* All OpenPilot family flight controllers (CopterControl, CC3D/CC3D Atom,
  Revoluition, Revoluition Nano):
  * OpenPilot 15.02.02 (latest CC/CC3D release)
  * OpenPilot 15.05.02 (latest Revolution release)
  * LibrePilot 15.09 (http://librepilot.org)
* All TauLabs 20151123 (http://taulabs.org) boards

### MAVLink module  

* APM 2.xx (ArduCopter, ArduPilot)
* Pixhawk PX4 (ArduCopter, ArduPilot)
* Paparazzi
* Generic MAVLink boards

### UBX module

* u-blox GPS modules - *experimental*

**Any help would be appreciated**

## Available indicators

* Altitude
* Climb rate
* Flight mode
* Armed indicator
* Connected indicator
* Flight time (time since FC boot actually)
* Roll angle
* Pitch angle
* GPS state
* GPS Latitude
* GPS Longitude
* Artifical horizon
* Throttle
* Groundspeed
* Battery voltage
* Battery current
* Battery consumed
* RSSI flag
* Distance to home
* Direction to home
* Callsign
* Temperature
* RSSI level
* Compass
* Airspeed

More to come.

## Build and install

[MultiOSD Building](doc/source/build.rst)

## Known bugs and problems

* No configuration software (https://github.com/UncleRus/MultiConf - 
  currently in development).
  
  But you can use [the serial console](doc/source/lowlevel/console.rst)
  to change [options](doc/source/lowlevel/options.rst) and screens layout.
* No autoconfig for UBX
* No mag support by UAVTalk module


