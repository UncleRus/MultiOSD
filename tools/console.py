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
import time
from serial.tools.miniterm import Miniterm


def reset (port):
    port.setDTR (False)
    port.setRTS (False)
    time.sleep (0.25)
    port.setDTR (True)
    port.setRTS (True)
    time.sleep (0.05)


def main (args):
    port = serial.Serial (args.port, 57600, timeout = args.timeout)
    
    print ('Press device reset button...')
    sys.stdout.flush ()
    
    reset (port)
    
    timeout = time.time () + args.timeout
    while True:
        if port.readline ().strip ().endswith ('READY'):
            break
        if time.time () >= timeout:
            raise IOError ('Sync timeout')

    port.write ('config\r\n')
    
    timeout = time.time () + args.timeout
    while True:
        l = port.readline ()
        if l.startswith ('MultiOSD'):
            print ('Connected. Type "help" for commands list. Press Ctrl+D to exit.')
            print ()
            print (l.strip ())
            break
        if time.time () >= timeout:
            raise IOError ('Sync timeout')
    sys.stdout.flush ()
    
    term = Miniterm (port, echo = False, eol = 'crlf', filters = ['default'], )
    
    term.exit_character = chr (0x04)
    term.menu_character = chr (0x14)
    term.raw = True
    term.set_rx_encoding ('Latin1')
    term.set_tx_encoding ('Latin1')

    term.start ()

    try:
        term.join (True)
    except KeyboardInterrupt:
        pass
    
    term.join ()
    print ()

    port.write ('exit\r\n')
    

if __name__ == '__main__':
    parser = argparse.ArgumentParser (description = 'MultiOSD config console')
    parser.add_argument ('-p', '--port', default = '/dev/ttyUSB0', help = 'Serial port name')
    parser.add_argument ('-t', '--timeout', type = float, default = 5.0, help = 'Timeout in seconds')
    try:
        main (parser.parse_args ())
    except KeyboardInterrupt:
        print ('Interrupted.')
    except Exception as e:
        print ('%s: %s' % (type (e).__name__, str (e)), file = sys.stderr)
