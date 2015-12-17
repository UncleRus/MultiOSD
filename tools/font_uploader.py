#!/usr/bin/env python
# -*- coding: utf-8 -*-
from __future__ import print_function
import serial
import argparse
import sys

def upload_font (args):
    f = open (args.font, 'rb')

    print ('Press device reset button...', end = ' ')
    sys.stdout.flush ()

    port = serial.Serial (args.port, 57600)
    while port.readline ().strip () != 'READY':
        pass

    port.write ('config\r\r')
    while port.readline ().strip () != 'osd#':
        pass

    print ('Connected.')
    print ('Uploading font...', end = ' ')
    sys.stdout.flush ()

    port.write ('font u\r')
    for line in iter (f.readline, ''):
        port.write (line)
    port.write ('\r\rreboot\r')
    port.close ()

    print ('Done.')


if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'MultiOSD font uploader')
    parser.add_argument ('-p', '--port', default = '/dev/ttyUSB0', help = 'Serial port name')
    parser.add_argument ('font', help = 'MCM font file')
    upload_font (parser.parse_args ())
