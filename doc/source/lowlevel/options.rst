****************
MultiOSD Options
****************


ADC
===

ADCREF
------

ADC reference voltage source. Make sure the **ADCUREF** value is correct.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+---------------+
| Value | Source        |
+=======+===============+
| 0     | AREF          |
+-------+---------------+
| 1     | AVCC          |
+-------+---------------+
| 3     | Internal 1.1V |
+-------+---------------+

ADCUREF
-------

Reference voltage, volts.

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+


MAX7456
=======

VMODE
-----

OSD composite video mode. Works only when jumper "PAL" on MinimOSD is not closed.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+------------+
| Value | Video mode |
+=======+============+
| 0     | PAL        |
+-------+------------+
| 1     | NTSC       |
+-------+------------+

VBRIGHT
-------

OSD character brithness in % of OSD white level.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+-------+-------+
| Value | White | Black |
+=======+=======+=======+
| 0     | 120%  | 0%    |
+-------+-------+-------+
| 1     | 100%  | 0%    |
+-------+-------+-------+
| 2     | 90%   | 0%    |
+-------+-------+-------+
| 3     | 80%   | 0%    |
+-------+-------+-------+
| 4     | 120%  | 10%   |
+-------+-------+-------+
| 5     | 100%  | 10%   |
+-------+-------+-------+
| 6     | 90%   | 10%   |
+-------+-------+-------+
| 7     | 80%   | 10%   |
+-------+-------+-------+
| 8     | 120%  | 20%   |
+-------+-------+-------+
| 9     | 100%  | 20%   |
+-------+-------+-------+
| 10    | 90%   | 20%   |
+-------+-------+-------+
| 11    | 80%   | 20%   |
+-------+-------+-------+
| 12    | 120%  | 30%   |
+-------+-------+-------+
| 13    | 100%  | 30%   |
+-------+-------+-------+
| 14    | 90%   | 30%   |
+-------+-------+-------+
| 15    | 80%   | 30%   |
+-------+-------+-------+


OSD
===

SCREENS
-------

Usable screens count. 1..8

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+


SWITCH
------

Way to switch between screens. Switching between screen works only when **SCREENS** > 1.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+------------------+
| Value | Switch by        |
+=======+==================+
| 0     | Never switch     |
+-------+------------------+
| 1     | By flight mode   |
+-------+------------------+
| 2     | By input channel |
+-------+------------------+

SWCH
----

Switch input channel. Works if **SWITCH** = 2.

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+


SWCHMIN
-------

Minimal pulse width of raw input channel (usually 1000). Works if
**SWITCH** = 2.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+


SWCHMAX
-------

Maximal pulse width of raw input channel (usually 2000). Works if
**SWITCH** = 2.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+


Telemetry
=========

MINCV
-----

Minimal cell voltage (dead battery), volts. 3.2 for LiPo

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

NOMCV
-----

Nominal cell voltage, volts. 3.7 for LiPo

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

MAXCV
-----

Maximal cell voltage, volts. 4.2 for LiPo

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

LOWCV
-----

Warning threshold, volts. About 3.5V for LiPo

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

CSIGN
-----

Callsign, 5-char text.

+------+---------+
| Type | Size    |
+======+=========+
| str  | 6 bytes |
+------+---------+


ADCBattery
==========

ABCS
----

Enable/disable current sensor.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+


ABVMUL
------

Voltage multiplier = Umax / **ADCUREF**.

For example, Umax = 16.8V, **ADCUREF** = 5.1V, **ABVMUL** = 16.8 / 5.1 = 3.294

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

ABCMUL
------

Current multiplier = Imax / **ADCUREF**.

For example, Imax = 90A, **ADCUREF** = 5.1V, **ABVMUL** = 90 / 5.1 = 17.647

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

ABUI
----

Interval between battery updates, ms. 200 is good enough.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+

ABVCH
-----

ATmega328p ADC channel for voltage sensor.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+-------------+------------+
| Value | ADC Channel | ATMega pin |
+=======+=============+============+
| 0     | 0           | 23         |
+-------+-------------+------------+
| 1     | 1           | 24         |
+-------+-------------+------------+
| 2     | 2           | 25         |
+-------+-------------+------------+
| 3     | 3           | 26         |
+-------+-------------+------------+
| 4     | 4           | 27         |
+-------+-------------+------------+
| 5     | 5           | 28         |
+-------+-------------+------------+
| 6     | 6           | 19         |
+-------+-------------+------------+
| 7     | 7           | 22         |
+-------+-------------+------------+


ABCCH
-----

ATmega328p ADC channel for current sensor.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+-------------+------------+
| Value | ADC Channel | ATMega pin |
+=======+=============+============+
| 0     | 0           | 23         |
+-------+-------------+------------+
| 1     | 1           | 24         |
+-------+-------------+------------+
| 2     | 2           | 25         |
+-------+-------------+------------+
| 3     | 3           | 26         |
+-------+-------------+------------+
| 4     | 4           | 27         |
+-------+-------------+------------+
| 5     | 5           | 28         |
+-------+-------------+------------+
| 6     | 6           | 19         |
+-------+-------------+------------+
| 7     | 7           | 22         |
+-------+-------------+------------+

ADCRSSI
=======

ARMUL
-----

RSSI multiplier = 100 / Umax, where Umax < **ADCUREF**.

For example, Umax = 5V, **ADCUREF** = 5.1, **ARMUL** = 100 / 5.0 = 20.0

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

ARCH
----

ATmega328p ADC channel for RSSI.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+-------------+------------+
| Value | ADC Channel | ATMega pin |
+=======+=============+============+
| 0     | 0           | 23         |
+-------+-------------+------------+
| 1     | 1           | 24         |
+-------+-------------+------------+
| 2     | 2           | 25         |
+-------+-------------+------------+
| 3     | 3           | 26         |
+-------+-------------+------------+
| 4     | 4           | 27         |
+-------+-------------+------------+
| 5     | 5           | 28         |
+-------+-------------+------------+
| 6     | 6           | 19         |
+-------+-------------+------------+
| 7     | 7           | 22         |
+-------+-------------+------------+

ARUI
----

Interval between RSSI updates, ms. 200 is good enough.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+

ARLT
----

RSSI warning threshold, percents.

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+


UAVTalk
=======

UTBRD
-----

Flight controller type. It's recommended to use correct FC type as it affects
altitide, velocity, temperature and other telemetry values.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+--------------+
| Value | FC type      |
+=======+==============+
| 0     | CC/CC3D/Atom |
+-------+--------------+
| 1     | Revo/Nano    |
+-------+--------------+


UTREL
-----

UAVTalk version release.

.. warning:: Incorrect value of this parameters may cause variuos bugs!

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+------------+----------+
| Value | Firmware   | Version  |
+=======+============+==========+
| 0     | OpenPilot  | 15.02.02 |
+-------+------------+----------+
| 1     | OpenPilot  | 15.05.02 |
+-------+------------+----------+
| 2     | LibrePilot | 15.09    |
+-------+------------+----------+


UTIHC
-----

Calculate home position internally if 1. Useful with CC3D/Atom.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+


MAVLink
=======

MLIBL
-----

Internal battery level calculation.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+


MLRLT
-----

RSSI warning threshold, percents.

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+

MLER
----

Emulate RSSI level. Useful when receiver has no RSSI level output.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+

MLERC
-----

Receiver channel used for emulation. Works only when **MLER** = 1.

+------+--------+
| Type | Size   |
+======+========+
| enum | 1 byte |
+------+--------+

+-------+----------------------+
| Value | Input channel        |
+=======+======================+
| 0     | Channel 1 (Roll)     |
+-------+----------------------+
| 1     | Channel 2 (Pitch)    |
+-------+----------------------+
| 2     | Channel 3 (Throttle) |
+-------+----------------------+
| 3     | Channel 4 (Yaw)      |
+-------+----------------------+
| 4     | Channel 5            |
+-------+----------------------+
| 5     | Channel 6            |
+-------+----------------------+
| 6     | Channel 7            |
+-------+----------------------+
| 7     | Channel 8            |
+-------+----------------------+


MLERT
-----

RSSI emulation threshold. Minimal pulse width of input channel, us. Works
only when **MLER** = 1. Typical value: 900

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+
