#!/usr/bin/env python
# -*- coding: utf-8 -*-

import serial
import sys

def read_font (filename):

    def get_byte (line):
        b = 0
        for i in xrange (8):
            b = (b << 1) | int (line [i])
        return b

    f = open (filename, 'rb')
    if f.readline ().strip () <> 'MAX7456':
        raise RuntimeError ('Unknown file format!')

    res = []
    for i in xrange (256):
        for row in xrange (54):
            res.append (get_byte (f.readline ()))
        for row in xrange (10):
            f.readline ()

    return res

font = read_font (sys.argv [1])

port = serial.Serial ('/dev/ttyS0', 57600)

print 'Press reset'

while port.readline ().strip () != 'READY':
    pass

port.write ('C')


while port.readline ().strip () != 'FONT':
    pass

port.write ('F')
for c in xrange (256):
    print c, ':', port.readline ().strip ()
    for byte in font [c * 54 : c * 54 + 54]:
        port.write (chr (byte))
