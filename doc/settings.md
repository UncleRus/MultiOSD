# Configuration options

## ADC settings

### ADCREF

ADC reference voltage source. Make sure that `ADCUREF` value is correct.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Source
----- | --------------
 0    | AREF          
 1    | AVCC          
 3    | Internal 1.1V 

### ADCUREF

ADC reference voltage, volts.

Type     | Size
-------- | --------
float    | 4 bytes


## MAX7456 settings

### VMODE

OSD composite video mode. Used only when jumper "PAL" on MinimOSD is not closed.
Value will be applied after OSD reboot.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Video mode
----- | ----------
 0    | PAL
 1    | NTSC

### VBRIGHT

OSD character brightness in % of OSD white level.
Value will be applied after OSD reboot.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | White | Black 
----- | ----- | -----
 0    | 120%  | 0%
 1    | 100%  | 0%
 2    | 90%   | 0%
 3    | 80%   | 0%
 4    | 120%  | 10%
 5    | 100%  | 10%
 6    | 90%   | 10%
 7    | 80%   | 10%
 8    | 120%  | 20%
 9    | 100%  | 20%
 10   | 90%   | 20%
 11   | 80%   | 20%
 12   | 120%  | 30%
 13   | 100%  | 30%
 14   | 90%   | 30%
 15   | 80%   | 30%


## OSD settings

### SCREENS

Usable screens count, 1..8

Type     | Size
-------- | --------
uint8    | 1 byte 

### SWM

Screen switch mode.

- **Selector** - Range of the input channel (`SELMAX` - `SELMIN`) divided by screens count `SCREENS`.
  Current screen selected by input channel value.
- **Toggle** - Cycle through the screens when the input channel value crossing threshold `TGTRESH`.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Mode
----- | -----------------
 0    | Selector mode
 1    | Toggle mode

### SWCH

Input channel used for screens switching.

Type     | Size
-------- | --------
uint8    | 1 byte 

### SELMIN

Minimal pulse width of the input channel (usually 1000). Used in **Selector** mode.

Type     | Size
-------- | --------
uint16   | 2 bytes

### SELMAX

Maximal pulse width of raw input channel (usually 2000). Used in **Selector** mode.

Type     | Size
-------- | --------
uint16   | 2 bytes

### TGTRESH

Screen switch threshold, microseconds. Used in **Toggle** mode. Recommended value is in range **1200 .. 1600**.

Type     | Size
-------- | --------
uint16   | 2 bytes


## Telemetry settings

### MINCV

Minimal cell voltage (dead battery), volts. Typical value for LiPo battery is **3.2**.

Type     | Size
-------- | --------
float    | 4 bytes

### NOMCV

Nominal cell voltage, volts. Typical value for LiPo battery is **3.7**.

Type     | Size
-------- | --------
float    | 4 bytes

### MAXCV

Maximal cell voltage, volts. Typical value for LiPo battery is **4.2**.

Type     | Size
-------- | --------
float    | 4 bytes

### LOWCV

Warning threshold, volts. Typical value for LiPo battery is about **3.5**.

Type     | Size
-------- | --------
float    | 4 bytes

### CSIGN

Text to show by callsign panel, 5 chars.

Type     | Size
-------- | --------
str      | 6 bytes


## ADCBattery module settings

### ABCS

Set this option to 1 to enable current sensor.

Type     | Size
-------- | --------
bool     | 1 byte

### ABV1MUL

Voltage multiplier = Umax / `ADCUREF` for flight battery.

For example, Umax = 16.8V, `ADCUREF` = 5.1V, `ABV1MUL` = 16.8 / 5.1 = 3.294

Type     | Size
-------- | --------
float    | 4 bytes

### ABV2MUL

Voltage multiplier = Umax / `ADCUREF` for secondary battery.

Type     | Size
-------- | --------
float    | 4 bytes

### ABCMUL

Current multiplier = Imax / `ADCUREF`

For example, Imax = 90A, `ADCUREF` = 5.1V, `ABCMUL` = 90 / 5.1 = 17.647

Type     | Size
-------- | --------
float    | 4 bytes

### ABV1CH

ATmega328p ADC channel for voltage sensor of the flight battery.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | ADC Channel | ATMega pin
----- | ----------- | ----------
 0    | 0           | 23
 1    | 1           | 24
 2    | 2           | 25
 3    | 3           | 26
 4    | 4           | 27
 5    | 5           | 28
 6    | 6           | 19
 7    | 7           | 22


### ABV2CH

ATmega328p ADC channel for voltage sensor of the secondary battery.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | ADC Channel | ATMega pin
----- | ----------- | ----------
 0    | 0           | 23
 1    | 1           | 24
 2    | 2           | 25
 3    | 3           | 26
 4    | 4           | 27
 5    | 5           | 28
 6    | 6           | 19
 7    | 7           | 22

### ABCCH

ATmega328p ADC channel for current sensor.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | ADC Channel | ATMega pin
----- | ----------- | ----------
 0    | 0           | 23
 1    | 1           | 24
 2    | 2           | 25
 3    | 3           | 26
 4    | 4           | 27
 5    | 5           | 28
 6    | 6           | 19
 7    | 7           | 22


## ADCRSSI module settings

### ARMUL

RSSI multiplier = 100 / Umax, where Umax <= `ADCUREF`.

For example, Umax = 5V, `ADCUREF` = 5.1, `ARMUL` = 100 / 5.0 = 20.0

Type     | Size
-------- | --------
float    | 4 bytes

### ARCH

ATmega328p ADC channel for RSSI.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | ADC Channel | ATMega pin
----- | ----------- | ----------
 0    | 0           | 23
 1    | 1           | 24
 2    | 2           | 25
 3    | 3           | 26
 4    | 4           | 27
 5    | 5           | 28
 6    | 6           | 19
 7    | 7           | 22

### ARUI

Interval between RSSI updates, ms. 200 is good enough.

Type     | Size
-------- | --------
uint16   | 2 bytes

### ARLT

RSSI warning threshold, percents.

Type     | Size
-------- | --------
uint8    | 1 byte


## UAVTalk module settings

### UTBR

UART baudrate to communicate with FC. Typically is **3** (57600).

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Baudrate
----- | ---------
 0    | 9600
 1    | 19200
 2    | 38400
 3    | 57600
 4    | 115200

### UTREL

UAVTalk version release.

**Please pay attention to this parameter! Most of the glitches or freezes caused by incorrect UTREL value.**

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Firmware   | Version
----- | ---------- | ---------
 0    | OpenPilot  | 15.02.02
 1    | OpenPilot  | 15.05.02
 2    | LibrePilot | 15.09
 3    | TauLabs    | 20151123
 4    | LibrePilot | Next

### UTIHC

Calculate home position by OSD if 1. Useful with CC3D/Atom.
If 0, flight controller defines home position.

Type     | Size
-------- | --------
bool     | 1 byte


## MAVLink module settings

### MLBR

UART baudrate to communicate with FC. Typically is **57600**.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Baudrate
----- | ---------
 0    | 9600
 1    | 19200
 2    | 38400
 3    | 57600
 4    | 115200
 
### MLFC

Flight controller type. Useful with Cleanflight MAVLink telemetry: set it to **3** (APM) for
correct display of the current flight mode.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | FC type
----- | -------
 0    | Autodetect
 3    | APM
 9    | Paparazzi
 12   | PX4

### MLIBL

Calculate battery level by OSD, not by flight controller if set to 1. Setting this option to 1 can
improve accuracy of the battery level calculation.

Type     | Size
-------- | --------
bool     | 1 byte

### MLBLT

Battery level warning threshold, percents. Used when `MLIBL` is 0.

Type     | Size
-------- | --------
uint8    | 1 byte


### MLRLT

RSSI warning threshold, percents. Used when `MLER` is 0.

Type     | Size
-------- | --------
uint8    | 1 byte

### MLER

Emulate RSSI level by receiver input channel value. Useful when receiver
has no RSSI level output.
RSSI low level warning will be set if value of the input channel defined
by `MLERC` lower than `MLERT`.

Type     | Size
-------- | --------
bool     | 1 byte

### MLERC

Receiver channel for RSSI emulation. Used when `MLER` is 1.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Input channel
----- | ---------------------
 0    | Channel 1 (Roll)
 1    | Channel 2 (Pitch)
 2    | Channel 3 (Throttle)
 3    | Channel 4 (Yaw)
 4    | Channel 5
 5    | Channel 6
 6    | Channel 7
 7    | Channel 8

### MLERT

RSSI emulation threshold (minimal pulse width of the input channel), microseconds.
Used when `MLERT` = 1. Typical value is about **900**.

Type     | Size
-------- | --------
uint16   | 2 bytes


## UBX module settings

### UBXBR

UART baudrate to communicate with U-BLOX GPS module.

Type     | Size
-------- | --------
enum     | 1 byte 

Value | Baudrate
----- | ---------
 0    | 9600
 1    | 19200
 2    | 38400
 3    | 57600
 4    | 115200

### UBXTO

Connection timeout, ms. Recommended value is **1000**.

Type     | Size
-------- | --------
uint16   | 2 bytes
