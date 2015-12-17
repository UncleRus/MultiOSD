#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import sys

portname = '/dev/ttyUSB0' if len (sys.argv) < 2 else sys.argv [1]
filename = 'main_font.mcm' if len (sys.argv) < 3 else sys.argv [2]

port = serial.Serial (portname, 57600)

print 'Press reset'

while port.readline ().strip () != 'READY':
    pass

port.write ('config\r\r')
while port.readline ().strip () != 'osd#':
    pass

print 'font up'

port.write ('font u\r')

print 'uploading'

with open (filename, 'rb') as f:
    for line in iter (f.readline, ""):
        port.write (line)

print 'done'
