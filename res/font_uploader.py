#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import sys
import time


filename = 'main_font.mcm' if len (sys.argv) < 2 else sys.argv [1]

port = serial.Serial ('/dev/ttyUSB0', 57600)

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
