# -*- coding: utf-8 -*-

from .options import *


__all__ = ['options']


header = Section ('HEADER', 0x00,
    UInt16Option ('Magic', 0x00, 'Magic word', readonly = True),
    UInt16Option ('Version', 0x02, 'Firmware version', readonly = True),
)

#===============================================================================
# ADC
#===============================================================================

adc_0002 = Section ('ADC', 0x05,
    EnumOption ('Source', 0x00, 'Reference voltage source', {0: 'AREF', 1: 'AVCC', 3: 'Internal 1.1V'}),
    FloatOption ('Uref', 0x01, 'Reference voltage, V'),
)

#===============================================================================
# MAX7456
#===============================================================================

max7456_002 = Section ('MAX7456', 0x10,
    EnumOption ('Video mode', 0x00, 'OSD composite video mode', ('PAL', 'NTSC')),
    EnumOption ('Brightness', 0x01, 'OSD character brithness in % of OSD white level', (
        '120% white, 0% black',
        '100% white, 0% black',
        '90% white, 0% black',
        '80% white, 0% black',
        '120% white, 10% black',
        '100% white, 10% black',
        '90% white, 10% black',
        '80% white, 10% black',
        '120% white, 20% black',
        '100% white, 20% black',
        '90% white, 20% black',
        '80% white, 20% black',
        '120% white, 30% black',
        '100% white, 30% black',
        '90% white, 30% black',
        '80% white, 30% black',
    ))
)

#===============================================================================
# ADCBAttery
#===============================================================================

adc_battery_004 = Section ('ADCBattery', 0x15,
    BoolOption ('Current sensor', 0x00, 'Read current from sensor connected to ADC'),
    FloatOption ('Voltage multiplier', 0x01, 'Voltage multiplier = Umax / Uref'),
    FloatOption ('Current multiplier', 0x05, 'Current multiplier = Imax / Uref'),
    UInt16Option ('Update interval', 0x09, 'Interval between battery parameters updates, ms'),
    EnumOption ('Voltage ADC channel', 0x0b, 'Atmega328p ADC channel for voltage', ['Channel %d' % i for i in xrange (8)]),
    EnumOption ('Current ADC channel', 0x0c, 'Atmega328p ADC channel for current', ['Channel %d' % i for i in xrange (8)]),
    persistent = False
)

#===============================================================================
# UAVTalk
#===============================================================================

uavtalk_004 = Section ('UAVTalk', 0x30,
    EnumOption ('Board', 0x00, 'UAVTalk board type', ('CC/CC3D', 'REVO')),
    BoolOption ('Internal home calc', 0x01, 'Calculate home distance/direction internally'),
    persistent = False
)

#===============================================================================
# MAVLink
#===============================================================================

mavlink_004 = Section ('MAVLink', 0x30,
    BoolOption ('Internal battery level calc', 0x00, 'Internal battery level calculation'),
    persistent = False
)

#===============================================================================
# Telemetry
#===============================================================================

telemetry_004 = Section ('Telemetry', 0x40,
    UInt8Option ('Main module', 0x00, 'Main telemetry module', readonly = True),
    FloatOption ('Minimal cell voltage', 0x01, 'Minimal cell voltage (dead battery), V. 3.2 for LiPo'),
    FloatOption ('Nominal cell voltage', 0x05, 'Nominal cell voltage, V. 3.7 for LiPo'),
    FloatOption ('Maximal cell voltage', 0x09, 'Maximal cell voltage, V. 4.2 for LiPo'),
    FloatOption ('Cell low voltage', 0x0d, 'Warning threshold, V. About 3.5V for LiPo'),
    StrOption ('Callsign', 0x11, 'Just a tag', length = 5),
)

#===============================================================================
# OSD
#===============================================================================

osd_002 = Section ('OSD', 0x70,
    EnumOption ('Switch screen', 0x00, 'How to switch between screens', ('Never', 'By flight mode', 'By input channel')),
    EnumOption ('Input channel', 0x01, 'Input channel number for screen switch', ['Channel %d' % i for i in xrange (9)]),
    EnumOption ('Screens count', 0x02, 'Number of available screens', {i: '%d' % i for i in xrange (1, 9)}),
    UInt16Option ('Channel min', 0x03, 'Minimal pulse width of raw input channel (usually 1000)'),
    UInt16Option ('Channel max', 0x05, 'Maximal pulse width of raw input channel (usually 2000)'),
)


#===============================================================================
# versions
#===============================================================================

ver_0004 = Options (
    header,
    adc_0002,
    max7456_002,
    adc_battery_004,
    uavtalk_004,
    mavlink_004,
    telemetry_004,
    osd_002
)

#===============================================================================
# catalog
#===============================================================================

options = {
    '0004': ver_0004,
    '0005': ver_0004,
}

