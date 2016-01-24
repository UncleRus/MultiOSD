Console
=======

MultiOSD offers the low-level configuration console. To use it you'll need
terminal software and serial port (it can be any USB-UART connector, like FTDI
or Prolific pl2303).

1. Run your terminal software.
2. Connect MultiOSD to the serial port.
3. Open the port with baudrate **57600**.
4. Press reset button on MultiOSD.
5. Wait for message ``READY``.
6. Type ``config`` and press ``Enter``. Entered characters will not be displayed.
   If you were mistaken you'll see ``BOOT`` message. Just press reset "Reset" and
   try again.
7. After that you'll see the command prompt

::

   MultiOSD v0.12
   osd#
   
8. Type ``help`` to see commands.

Commands
--------

help
^^^^

Provides information about commands.

*Example*

::

   osd# help
   help - Commands list
   info - Firmware version, modules, panels
   opt - Read/write OSD options
   scr - View/edit screens layout
   reset - Reset settings to defaults
   font - Upload and download mcm-file
   adc - Read ADC values
   exit - Exit console
   reboot - Reboot OSD

info
^^^^

Provides information about firmware and shows the list of available screen panels.

*Example*

::

      osd# info
      VERSION: 0.12
      MODULES: ADCBattery ADCRSSI MAVLink
      PANELS:
      000: StableAlt
      001: Climb
      002: FlightMode
      003: ArmedFlag
      004: ConState
      005: FlightTime
      006: Roll
      007: Pitch
      008: GPS
      009: Lat
      010: Lon
      011: Horizon
      012: Throttle
      013: GroundSpeed
      014: BatVoltage
      015: BatCurrent
      016: BatConsumed
      017: RSSIFlag
      018: HomeDistance
      019: HomeDirection
      020: CallSign
      021: Temperature
      022: RSSI
      023: Compass

exit
^^^^

Exit console and continue boot to OSD.

reboot
^^^^^^

Reboot MinimOSD.

opt
^^^

Read and write :doc:`options`.

opt l
"""""

List all the options with their values.

*Example*

::

   osd# opt l
   0x10    (byte:1@)       VMODE   = 0
   0x11    (byte:1@)       VBRIGHT = 0
   0x40    (float:4@)      MINCV   = 3.2000
   0x44    (float:4@)      NOMCV   = 3.7000
   0x48    (float:4@)      MAXCV   = 4.2000
   0x4c    (float:4@)      LOWCV   = 3.5000
   0x50    (str:6@)        CSIGN   = -----
   0x5     (byte:1@)       ADCREF  = 3
   0x6     (float:4@)      ADCUREF = 1.1000
   0x15    (bool:1@)       ABCS    = 1
   0x16    (float:4@)      ABVMUL  = 10.9800
   0x1a    (float:4@)      ABCMUL  = 18.1818
   0x1e    (word:2@)       ABUI    = 200
   0x20    (byte:1@)       ABVCH   = 6
   0x21    (byte:1@)       ABCCH   = 7
   0x28    (byte:1@)       ARCH    = 1
   0x29    (word:2@)       ARUI    = 200
   0x2b    (byte:1@)       ARLT    = 10
   0x2c    (float:4@)      ARMUL   = 20.0000
   0x30    (byte:1@)       MLIBL   = 1
   0x31    (byte:1@)       MLRLT   = 10
   0x32    (byte:1@)       MLER    = 0
   0x33    (byte:1@)       MLERC   = 2
   0x34    (word:2@)       MLERT   = 920
   0x70    (byte:1@)       SWITCH  = 2
   0x71    (byte:1@)       SWCH    = 8
   0x72    (byte:1@)       SCREENS = 8
   0x73    (word:2@)       SWCHMIN = 300
   0x75    (word:2@)       SWCHMAX = 1700


opt g
"""""

Read an option value from EEPROM.

Usage: ``opt g <name>``
   - ``name`` - option name, case insensetive

*Example*

::

   osd# opt g ABCMUL
   0x1a    (float:4@)      ABCMUL  = 18.1818

opt s
"""""

Write an option value to EEPROM.

Usage: ``opt s <name> <value>``
   - ``name`` - option name, case insensetive
   - ``value`` - option value

*Example*

::

   osd# opt s ABCMUL 19.43
   0x1a    (float:4@)      ABCMUL  = 19.4300

scr
^^^

Setup the OSD screens layout using panels.

scr d
"""""

Dump current screen layout.

Usage: ``scr d [ <screen> ]``.
   - ``screen`` - screen number, 0..7. Without this arg will dumps all the screens.

*Example*

::

   osd# scr d 0
   Screen 0
   #       x       y       panel   panel name
   --      --      --      --      --
   0       1       1       8       GPS
   1       8       1       9       Lat
   2       19      1       10      Lon
   3       0       2       4       ConState
   4       3       2       3       ArmedFlag
   5       21      3       22      RSSI
   6       24      2       2       FlightMode
   7       1       6       7       Pitch
   8       8       6       11      Horizon
   9       23      6       12      Throttle
   10      1       8       13      GroundSpeed
   11      23      8       0       StableAlt
   12      1       10      6       Roll
   13      23      10      1       Climb
   14      1       13      5       FlightTime
   15      9       12      23      Compass
   16      1       14      18      HomeDistance
   17      8       14      19      HomeDirection
   18      22      12      14      BatVoltage
   19      22      13      15      BatCurrent
   20      22      14      16      BatConsumed

scr a
"""""

Append panel to the screen layout.

Usage: ``scr a <screen> <x> <y> <panel>``
   - ``screen`` - screen number, 0..7
   - ``x`` - X-cordinate of the panel
   - ``y`` - Y-cordinate of the panel
   - ``panel`` - panel index listed in ``info`` command output.

*Example*

::

   osd# scr d 7
   Screen 7
   #       x       y       panel   panel name
   --      --      --      --      --

   osd# scr a 7 2 3 17
   Screen 7
   #       x       y       panel   panel name
   --      --      --      --      --
   0       2       3       17      RSSIFlag

scr e
"""""

Edit screen panel.

Usage: ``scr e <screen> <panel #> <x> <y> <panel>``
   - ``screen`` - screen number, 0..7
   - ``panel #`` - panel index in screen layout
   - ``x`` - X-cordinate of the panel
   - ``y`` - Y-cordinate of the panel
   - ``panel`` - panel index listed in ``info`` command output.

*Example*

::

   osd# scr d 2
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --
   0       0       0       4       ConState
   1       3       0       3       ArmedFlag
   2       20      1       22      RSSI
   3       24      0       2       FlightMode
   4       1       6       7       Pitch
   5       1       8       0       StableAlt
   6       1       10      6       Roll
   7       1       13      5       FlightTime
   8       22      14      14      BatVoltage

   osd# scr e 2 0 1 1 21
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --
   0       1       1       21      Temperature
   1       3       0       3       ArmedFlag
   2       20      1       22      RSSI
   3       24      0       2       FlightMode
   4       1       6       7       Pitch
   5       1       8       0       StableAlt
   6       1       10      6       Roll
   7       1       13      5       FlightTime
   8       22      14      14      BatVoltage


scr r
"""""

Remove panel from the screen layout.

Usage: ``scr r <screen> <panel #>``
   - ``screen`` - screen number, 0..7
   - ``panel #`` - panel index in screen layout

*Example*

::

   osd# scr d 2
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --
   0       1       1       21      Temperature
   1       3       0       3       ArmedFlag
   2       20      1       22      RSSI
   3       24      0       2       FlightMode
   4       1       6       7       Pitch
   5       1       8       0       StableAlt
   6       1       10      6       Roll
   7       1       13      5       FlightTime
   8       22      14      14      BatVoltage

   osd# scr r 2 4
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --
   0       1       1       21      Temperature
   1       3       0       3       ArmedFlag
   2       20      1       22      RSSI
   3       24      0       2       FlightMode
   4       1       8       0       StableAlt
   5       1       10      6       Roll
   6       1       13      5       FlightTime
   7       22      14      14      BatVoltage

scr c
"""""

Clear screen.

Usage: ``scr c <screen>``
   - ``screen`` - screen number, 0..7

*Example*

::

   osd# scr d 2
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --
   0       1       1       21      Temperature
   1       3       0       3       ArmedFlag
   2       20      1       22      RSSI
   3       24      0       2       FlightMode
   4       1       8       0       StableAlt
   5       1       10      6       Roll
   6       1       13      5       FlightTime
   7       22      14      14      BatVoltage

   osd# scr c 2
   Screen 2
   #       x       y       panel   panel name
   --      --      --      --      --

font
^^^^

Upload and download MAX7456 MCM-font.

font u
""""""

Upload MCM-file.

font d
""""""

Download current MAX7456 font as MCM-file.

adc
^^^

Read ADC values.

Uasge: ``adc [<channel>]``
   - ``channel`` - ADC channel numer, 0..15

*Example*

::

   osd# adc
   ADC 0   0.4877 (454)
   ADC 1   0.7391 (688)
   ADC 2   0.9346 (870)
   ADC 3   1.0581 (985)
   ADC 4   1.0989 (1023)
   ADC 5   1.0989 (1023)
   ADC 6   1.0989 (1023)
   ADC 7   1.0989 (1023)
   ADC 8   0.3803 (354)
   ADC 9   0.0021 (2)
   ADC 10  0.8594 (800)
   ADC 11  0.0000 (0)
   ADC 12  0.8551 (796)
   ADC 13  0.0000 (0)
   ADC 14  0.8583 (799)
   ADC 15  0.0000 (0)

