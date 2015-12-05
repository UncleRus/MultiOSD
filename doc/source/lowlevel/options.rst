****************
MultiOSD Options
****************


ADC
===

.. _o-adcref:

ADCREF
------

ADC reference voltage source. Make sure the :ref:`o-adcuref` value is correct.

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

.. _o-adcuref:

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

.. _o-vmode:

VMODE
-----

OSD composite video mode. Works only when jumper "PAL" on MinimOSD is not closed.
Value will be applied after reboot.

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

.. _o-vbright:

VBRIGHT
-------

OSD character brithness in % of OSD white level.
Value will be applied after MinimOSD reboot.

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

.. _o-screens:

SCREENS
-------

Used screens count. 1..8

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+

.. _o-switch:

SWITCH
------

Way to switch between screens. Switching between screen works only when :ref:`o-screens` > 1.

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

.. _o-swch:

SWCH
----

Switch input channel. Works if :ref:`o-switch` = 2.

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+

.. _o-swchmin:

SWCHMIN
-------

Minimal pulse width of raw input channel (usually 1000). Works if
:ref:`o-switch` = 2.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+

.. _o-swchmax:

SWCHMAX
-------

Maximal pulse width of raw input channel (usually 2000). Works if
:ref:`o-switch` = 2.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+


Telemetry
=========

.. _o-mincv:

MINCV
-----

Minimal cell voltage (dead battery), volts. Typical value for LiPo battery
is **3.2**.

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-nomcv:

NOMCV
-----

Nominal cell voltage, volts. Typical value for LiPo battery is **3.7**.

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-maxcv:

MAXCV
-----

Maximal cell voltage, volts. Typical value for LiPo battery is **4.2**.

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-lowcv:

LOWCV
-----

Warning threshold, volts. Typical value for LiPo battery is about **3.5**.

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-csign:

CSIGN
-----

Text to show by callsign panel, 5 chars.

+------+---------+
| Type | Size    |
+======+=========+
| str  | 6 bytes |
+------+---------+

ADCBattery
==========

.. _o-abcs:

ABCS
----

1 to enable current sensor.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+

.. _o-abvmul:

ABVMUL
------

Voltage multiplier = Umax / :ref:`o-adcuref`.

For example, Umax = 16.8V, :ref:`o-adcuref` = 5.1V, :ref:`o-abvmul` = 16.8 / 5.1 = 3.294

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-abcmul:

ABCMUL
------

Current multiplier = Imax / :ref:`o-adcuref`

For example, Imax = 90A, :ref:`o-adcuref` = 5.1V, :ref:`o-abcmul` = 90 / 5.1 = 17.647

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-abui:

ABUI
----

Interval between battery stats updates, ms. 200 is good enough.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+

.. _o-abvch:

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


.. _o-abcch:

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

.. _o-armul:

ARMUL
-----

RSSI multiplier = 100 / Umax, where Umax <= :ref:`o-adcuref`.

For example, Umax = 5V, :ref:`o-adcuref` = 5.1, :ref:`o-armul` = 100 / 5.0 = 20.0

+-------+---------+
| Type  | Size    |
+=======+=========+
| float | 4 bytes |
+-------+---------+

.. _o-arch:

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

.. _o-arui:

ARUI
----

Interval between RSSI updates, ms. 200 is good enough.

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+

.. _o-arlt:

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

.. _o_utbrd:

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

.. _o_utrel:

UTREL
-----

UAVTalk version release.

.. warning:: Incorrect value of this parameters can cause variuos bugs!

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


.. _o-utihc:

UTIHC
-----

Calculate home position internally if 1. Useful with CC3D/Atom.
If 0, home position will be taken from the flight controller.

.. note:: Values other than 0 aren't supported now.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+

MAVLink
=======

.. _o-mlibl:

MLIBL
-----

Claculate battery level internally if 1. Setting this option to 1 can
improve the accuracy of the battery level calculation.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+

.. _o-mlrlt:

MLRLT
-----

RSSI warning threshold, percents. This option will work only :ref:`o-mler` = 0.

+-------+--------+
| Type  | Size   |
+=======+========+
| uint8 | 1 byte |
+-------+--------+

.. _o-mler:

MLER
----

Emulate RSSI level by receiver input channel value. Useful when receiver
has no RSSI level output.
RSSI low level warning will be set if value if the input channel defined
by :ref:`o-mlerc` lower than :ref:`o-mlert`.

+------+--------+
| Type | Size   |
+======+========+
| bool | 1 byte |
+------+--------+

.. _o-mlerc:

MLERC
-----

Receiver channel used for emulation. Works only when :ref:`o-mler` = 1.

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

.. _o-mlert:

MLERT
-----

RSSI emulation threshold. Minimal pulse width of input channel, us. Works
only when :ref:`o-mlert` = 1. Typical value: ~900

+--------+---------+
| Type   | Size    |
+========+=========+
| uint16 | 2 bytes |
+--------+---------+
