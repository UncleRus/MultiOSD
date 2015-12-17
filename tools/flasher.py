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
import time
import struct
import random
import intelhex
from io import BytesIO
import argparse
import sys


class const (object):

    RESP_STK_OK       = b'\x10'
    RESP_STK_FAILED   = b'\x11'
    RESP_STK_UNKNOWN  = b'\x12'
    RESP_STK_NODEVICE = b'\x13'
    RESP_STK_INSYNC   = b'\x14'
    RESP_STK_NOSYNC   = b'\x15'

    RESP_ADC_CHANNEL_ERROR = b'\x16'
    RESP_ADC_MEASURE_OK    = b'\x17'
    RESP_PWM_CHANNEL_ERROR = b'\x18'
    RESP_PWM_ADJUST_OK     = b'\x19'

    SYNC_CRC_EOP = b'\x20'  # 'SPACE'

    CMD_STK_GET_SYNC    = b'\x30'
    CMD_STK_GET_SIGN_ON = b'\x31'

    CMD_STK_SET_PARAMETER  = b'\x40'
    CMD_STK_GET_PARAMETER  = b'\x41'
    CMD_STK_SET_DEVICE     = b'\x42'
    CMD_STK_SET_DEVICE_EXT = b'\x45'

    CMD_STK_ENTER_PROGMODE  = b'\x50'
    CMD_STK_LEAVE_PROGMODE  = b'\x51'
    CMD_STK_CHIP_ERASE      = b'\x52'
    CMD_STK_CHECK_AUTOINC   = b'\x53'
    CMD_STK_LOAD_ADDRESS    = b'\x55'
    CMD_STK_UNIVERSAL       = b'\x56'
    CMD_STK_UNIVERSAL_MULTI = b'\x57'

    CMD_STK_PROG_FLASH    = b'\x60'
    CMD_STK_PROG_DATA     = b'\x61'
    CMD_STK_PROG_FUSE     = b'\x62'
    CMD_STK_PROG_LOCK     = b'\x63'
    CMD_STK_PROG_PAGE     = b'\x64'
    CMD_STK_PROG_FUSE_EXT = b'\x65'

    CMD_STK_READ_FLASH      = b'\x70'
    CMD_STK_READ_DATA       = b'\x71'
    CMD_STK_READ_FUSE       = b'\x72'
    CMD_STK_READ_LOCK       = b'\x73'
    CMD_STK_READ_PAGE       = b'\x74'
    CMD_STK_READ_SIGN       = b'\x75'
    CMD_STK_READ_OSCCAL     = b'\x76'
    CMD_STK_READ_FUSE_EXT   = b'\x77'
    CMD_STK_READ_OSCCAL_EXT = b'\x78'

    PARAM_STK_HW_VER         = b'\x80'  # ' ' - R
    PARAM_STK_SW_MAJOR       = b'\x81'  # ' ' - R
    PARAM_STK_SW_MINOR       = b'\x82'  # ' ' - R
    PARAM_STK_LEDS           = b'\x83'  # ' ' - R/W
    PARAM_STK_VTARGET        = b'\x84'  # ' ' - R/W
    PARAM_STK_VADJUST        = b'\x85'  # ' ' - R/W
    PARAM_STK_OSC_PSCALE     = b'\x86'  # ' ' - R/W
    PARAM_STK_OSC_CMATCH     = b'\x87'  # ' ' - R/W
    PARAM_STK_RESET_DURATION = b'\x88'  # ' ' - R/W
    PARAM_STK_SCK_DURATION   = b'\x89'  # ' ' - R/W
    PARAM_STK_BUFSIZEL       = b'\x90'  # ' ' - R/W, Range {0..255}
    PARAM_STK_BUFSIZEH       = b'\x91'  # ' ' - R/W, Range {0..255}
    PARAM_STK_DEVICE         = b'\x92'  # ' ' - R/W, Range {0..255}
    PARAM_STK_PROGMODE       = b'\x93'  # ' ' - 'P' or 'S'
    PARAM_STK_PARAMODE       = b'\x94'  # ' ' - TRUE or FALSE
    PARAM_STK_POLLING        = b'\x95'  # ' ' - TRUE or FALSE
    PARAM_STK_SELFTIMED      = b'\x96'  # ' ' - TRUE or FALSE
    PARAM_STK_TOPCARD_DETECT = b'\x98'  # ' ' - Detect top-card attached

    STATE_STK_INSYNC     = b'\x01'  # INSYNC status bit, '1' - INSYNC
    STATE_STK_PROGMODE   = b'\x02'  # Programming mode,  '1' - PROGMODE
    STATE_STK_STANDALONE = b'\x04'  # Standalone mode,   '1' - SM mode
    STATE_STK_RESET      = b'\x08'  # RESET button,      '1' - Pushed
    STATE_STK_PROGRAM    = b'\x10'  # Program button, '   1' - Pushed
    STATE_STK_LEDG       = b'\x20'  # Green LED status,  '1' - Lit
    STATE_STK_LEDR       = b'\x40'  # Red LED status,    '1' - Lit
    STATE_STK_LEDBLINK   = b'\x80'  # LED blink ON/OFF,  '1' - Blink

    MEM_EEPROM = 0x45
    MEM_FLASH  = 0x46


def need_connection (func):
    def wrapped (*args, **kwargs):
        self = args [0]
        if not self.serial.isOpen ():
            raise IOError ('Not connected')
        return func (*args, **kwargs)
    return wrapped


def dump (data):
    return ' '.join (('%02x' % ord (b) for b in data))


def get_byte (bytes_, i = 0):
    return bytes_ [i] if isinstance (bytes_ [i], int) else ord (bytes_ [i])


class STK500 (object):

    def __init__ (self, timeout = 5.0):
        self.serial = serial.Serial ()
        self.timeout = timeout

    def display_progress (self, message, value):
        if value > 100:
            value = 100
        if value < 0:
            value = 0
        bar = '[%-25s] %3d%%' % ('#' * int (round (value / 4.0)), value)
        print ((message % bar), end = '\r')
        sys.stdout.flush ()

    def read_bytes (self, length):
        stop = time.time () + self.timeout

        if length is None:
            res = bytearray ()
            while True:
                if time.time () > stop:
                    raise IOError ('Sync timeout')
                if self.serial.inWaiting () > 0:
                    b = self.serial.read ()
                    res.append (b)
                    if b == const.RESP_STK_OK:
                        return bytes (res)

        while self.serial.inWaiting () < length:
            if time.time () > stop:
                raise IOError ('Sync timeout')
        return self.serial.read (length)

    def sync (self, length = 0):
        res = self.read_bytes (None if length is None else length + 2)
        #print 'RECV (%02x):' % len (res), dump (res)
        if res [0] != const.RESP_STK_INSYNC or res [-1] != const.RESP_STK_OK:
            raise IOError ('Out of sync')
        return res [1:-1]

    def execute (self, cmd, res_length = 0):
        #print 'SENT: (%02x):' % len (cmd + const.SYNC_CRC_EOP), dump (cmd + const.SYNC_CRC_EOP)
        self.serial.write (cmd)
        self.serial.write (const.SYNC_CRC_EOP)
        time.sleep (0.05)
        return self.sync (res_length)

    def get_parameter (self, parameter, size = 1):
        return self.execute (const.CMD_STK_GET_PARAMETER + parameter, size)

    def iterate_command (self, state, address, length, func, buffer_size):
        loops = length / buffer_size
        tail = length % buffer_size
        if tail:
            loops += 1
        result = bytearray ()
        self.display_progress (state, 0)
        for i in xrange (loops):
            offset = i * buffer_size
            self.set_address (address + offset)
            result += func (offset, tail if tail and i == loops - 1 else buffer_size)
            self.display_progress (state, int (100.0 / loops * (i + 1)))
        self.display_progress (state, int (100.0 / loops * (i + 1)))
        print ()
        return bytes (result)

    def read_memory (self, address, length, eeprom = False):
        return self.iterate_command (
            'Reading %s: %%s' % ('EEPROM' if eeprom else 'flash'),
            address,
            length,
            lambda _, size: self.execute (const.CMD_STK_READ_PAGE + struct.pack ('>HB', size, const.MEM_EEPROM if eeprom else const.MEM_FLASH), size),
            0x100
        )

    def write_memory (self, address, data, eeprom = False):
        return self.iterate_command (
            'Writing %s: %%s' % ('EEPROM' if eeprom else 'flash'),
            address,
            len (data),
            lambda offs, size: self.execute (const.CMD_STK_PROG_PAGE + struct.pack ('>HB', size, const.MEM_EEPROM if eeprom else const.MEM_FLASH) + data [offs:offs + size], 0),
            0x80
        )

    def reset (self):
        self.serial.setDTR (False)
        self.serial.setRTS (False)
        time.sleep (0.25)
        self.serial.setDTR (True)
        self.serial.setRTS (True)
        time.sleep (0.05)

    def open (self, port, baudrate = 115200):
        print ('Connecting. Press device reset button...')
        try:
            self.serial.port = port
            self.serial.baudrate = baudrate
            self.serial.parity = serial.PARITY_NONE
            self.serial.stopbits = serial.STOPBITS_ONE
            self.serial.datasize = serial.EIGHTBITS
            self.serial.timeout = self.timeout
            self.serial.open ()

            self.reset ()

            for _i in xrange (10):
                self.serial.write (const.CMD_STK_GET_SYNC + const.SYNC_CRC_EOP)
                self.serial.flushInput ()
                time.sleep (0.01)

            stop = time.time () + self.timeout
            while True:
                if time.time () > stop:
                    raise IOError ('Sync timeout')
                self.serial.flushInput ()
                self.serial.write (const.CMD_STK_GET_SYNC + const.SYNC_CRC_EOP)
                time.sleep (random.randrange (500, 1000) / 1000.0)
                if self.serial.inWaiting () < 2:
                    continue
                if self.read_bytes (2) == const.RESP_STK_INSYNC + const.RESP_STK_OK:
                    break
            bl_v_h, bl_v_l = self.get_bootloader_version ()
            print ('Connected. BL version: %d.%d, chip signature: %4x' % (bl_v_h, bl_v_l, self.read_signature ()))
        except:
            try:
                self.serial.close ()
            except:
                pass
            raise

    def close (self):
        self.serial.close ()
        print ('Disonnected.')

    @need_connection
    def get_sign_on (self):
        '''
        Not supported by Optiboot
        '''
        return self.execute (const.CMD_STK_GET_SIGN_ON, None)

    @need_connection
    def get_bootloader_version (self):
        '''
        4.4 on optiboot
        '''
        return (
            get_byte (self.get_parameter (const.PARAM_STK_SW_MAJOR)),
            get_byte (self.get_parameter (const.PARAM_STK_SW_MINOR))
        )

    @need_connection
    def read_signature (self):
        return struct.unpack ('>I', b'\x00' + self.execute (const.CMD_STK_READ_SIGN, 3))[0]

    @need_connection
    def set_address (self, address):
        self.execute (const.CMD_STK_LOAD_ADDRESS + struct.pack ('<H', address >> 1))

    @need_connection
    def enter_prog_mode (self):
        self.execute (const.CMD_STK_ENTER_PROGMODE)

    @need_connection
    def leave_prog_mode (self):
        self.execute (const.CMD_STK_LEAVE_PROGMODE)

    @need_connection
    def download_eeprom (self, address, length):
        return self.read_memory (address, length, True)

    @need_connection
    def upload_eeprom (self, address, data):
        return self.write_memory (address, data, True)

    @need_connection
    def download_flash (self, address, length):
        return self.read_memory (address, length, False)

    @need_connection
    def upload_flash (self, address, data):
        return self.write_memory (address, data, False)

    @need_connection
    def upload_hex (self, file):
        print ('Uploading firmware...')
        buffer = BytesIO ()
        intelhex.hex2bin (file, buffer)
        bin = buffer.getvalue ()
        self.upload_flash (0, bin)
        print ('Verifying...')
        if self.download_flash (0, len (bin)) != bin:
            raise IOError ('Verification failed')
        print ('Done.')


def main ():
    parser = argparse.ArgumentParser (description = 'MultiOSD firmware uploader')
    parser.add_argument ('-p', '--port', default = '/dev/ttyUSB0', help = 'Serial port name')
    parser.add_argument ('hex', help = 'Firmware filename')
    args = parser.parse_args ()

    try:
        s = STK500 ()
        s.open (args.port)
        s.upload_hex (open (args.hex, 'rb'))
    except KeyboardInterrupt:
        print ('Interrupted.')
    except Exception as e:
        print ('%s: %s' % (type (e).__name__, str (e)), file = sys.stderr)


if __name__ == '__main__':
    main ()


