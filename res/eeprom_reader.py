#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import sys


port = serial.Serial ('/dev/ttyUSB0', 57600)

print 'Press reset'

while port.readline ().strip () != 'READY':
    pass

port.write ('config\r\r')
while port.readline ().strip () != 'osd#':
    pass

port.write ('eeprom r\r')
port.readline ()

with open (sys.argv [1], 'wb') as f:
    for i in xrange (0x400):
        f.write (port.read (1))

print 'done'

