## Console connection

MultiOSD offers the low-level configuration console. To use it you need
terminal software and serial port (it can be any USB-UART connector, like FTDI
or Prolific pl2303).

* Run your terminal software.
* Connect MultiOSD to the serial port.
* Open the port with baudrate **57600**.
* Press reset button on MultiOSD.
* Wait for message **`READY`**.
* Type **`config`** and press `Enter`. Entered characters will not be displayed.
  If you were mistaken you'll see **`BOOT`** message. Just press reset and
  try again.

_**OR**_

* Use the tool: 

        python tools/console.py -p <port> --timeout <seconds>
   
   For example:

        python tools/console.py -p /dev/ttyUSB0 --timeout 10

* After that you'll see the command prompt

        MultiOSD v0.12
        osd#

* Type **`help`** to see commands.


## Console commands

### help

Provides information about commands.

*Example*

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

### info

Provides information about firmware and shows the list of available screen panels.

*Example*

    osd# info
    VERSION: 0010
    MODULES: ADCBattery ADCRSSI MAVLink
    PANELS:
    000: StableAlt
    001: Climb
    002: FlightMode
    003: ArmedFlag
    ...
    023: Compass

### exit

Exit console and run OSD.

### reboot

Reboot MinimOSD.

### opt

Read and write [options](Configuration-options).

#### opt l

List all the options with their values.

*Example*

    osd# opt l
    0x10    (byte:1)        VMODE   = 0
    0x11    (byte:1)        VBRIGHT = 0
    0x50    (str:6)         CSIGN   = -----
    ...
    0x75    (word:2)        SWCHMAX = 1700

#### opt g

Read option value from EEPROM.

*Usage:* `opt g <name>`
   - `name` - case insensitive option name

*Example*

    osd# opt g abcmul
    0x1a    (float:4)       ABCMUL  = 18.1818

#### opt s

Write option value to EEPROM.

*Usage:* `opt s <name> <value>`
   - `name` - case insensitive option name
   - `value` - option value

*Example*

    osd# opt s abcmul 19.43
    0x1a    (float:4)       ABCMUL  = 19.4300

### scr

Setup the OSD screens layout using panels.

#### scr d

Dump current screen layout.

*Usage:* `scr d [ <screen> ]`.
   - `screen` - screen number, 0..7. All screens will be dumped when no argument supplied.

*Example*

    osd# scr d 0
    Screen 0
    #       x       y       panel   panel name
    --      --      --      --      --
    0       1       1       8       GPS
    1       8       1       9       Lat
    ...
    19      22      13      15      BatCurrent
    20      22      14      16      BatConsumed

#### scr a

Append panel to the screen layout.

*Usage:* `scr a <screen> <x> <y> <panel>`
   - `screen` - screen number, 0..7
   - `x` - X-cordinate of the panel
   - `y` - Y-cordinate of the panel
   - `panel` - panel index listed in `info` command output.

*Example*

    osd# scr d 7
    Screen 7
    #       x       y       panel   panel name
    --      --      --      --      --

    osd# scr a 7 2 3 17
    Screen 7
    #       x       y       panel   panel name
    --      --      --      --      --
    0       2       3       17      RSSIFlag

#### scr e

Edit screen panel.

*Usage:* `scr e <screen> <panel #> <x> <y> <panel>`
   - `screen` - screen number, 0..7
   - `panel #` - panel index in screen layout
   - `x` - X-cordinate of the panel
   - `y` - Y-cordinate of the panel
   - `panel` - panel index listed in `info` command output.

*Example*

    osd# scr d 2
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --
    0       0       0       4       ConState
    1       3       0       3       ArmedFlag
    2       20      1       22      RSSI
    3       24      0       2       FlightMode
    4       1       6       7       Pitch

    osd# scr e 2 0 1 1 21
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --
    0       1       1       21      Temperature
    1       3       0       3       ArmedFlag
    2       20      1       22      RSSI
    3       24      0       2       FlightMode
    4       1       6       7       Pitch

#### scr r

Remove panel from the screen layout.

*Usage:* `scr r <screen> <panel #>`
   - `screen` - screen number, 0..7
   - `panel #` - panel index in screen layout

*Example*

    osd# scr d 2
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --
    0       1       1       21      Temperature
    1       3       0       3       ArmedFlag
    2       20      1       22      RSSI
    3       24      0       2       FlightMode
    4       1       6       7       Pitch

    osd# scr r 2 3
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --
    0       1       1       21      Temperature
    1       3       0       3       ArmedFlag
    2       20      1       22      RSSI
    3       1       6       7       Pitch

#### scr c

Clear screen.

Usage: `scr c <screen>`
   - `screen` - screen number, 0..7

*Example*

    osd# scr d 2
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --
    0       1       1       21      Temperature
    1       3       0       3       ArmedFlag
    2       20      1       22      RSSI
    3       1       6       7       Pitch

    osd# scr c 2
    Screen 2
    #       x       y       panel   panel name
    --      --      --      --      --

### font

Upload and download MAX7456 MCM-font.

#### font u

Upload MCM-file.

#### font d

Download current MAX7456 font as MCM-file.

### adc

Read ADC values.

*Usage:* `adc [ <channel> ]`
   - `channel` - ADC channel numer, 0..15

*Example*

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
