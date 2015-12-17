#!/usr/bin/env python
# -*- coding: utf-8 -*-

# This file is part of MultiOSD <https://github.com/UncleRus/MultiOSD>
#
# MultiOSD is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

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
