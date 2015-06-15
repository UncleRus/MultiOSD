# -*- coding: utf-8 -*-

from serial import Serial
import time
import re
from . import catalog
from . import const


def _connection (func):
    def _wrapped (*args, **kwargs):
        if not args [0].connected:
            raise IOError ('Board not found')
        return func (*args, **kwargs)
    return _wrapped


class Board (object):

    osd_prompt = 'osd#'

    info_regexp = re.compile (r'([A-Z]+):\s*(.*)?')

    def __init__ (self, port = '/dev/ttyUSB0', baudrate = 57600):
        self.port = port
        self.baudrate = baudrate
        self._reset ()

    def _reset (self):
        self.connected = False
        self.modules = []
        self.panels = []
        self.version = None
        self.eeprom = bytearray ([0xff] * const.EEPROM_SIZE)
        self.options = None

    def connect (self, timeout = 5):
        if self.connected:
            self.disconnect ()

        self.serial = Serial (self.port, self.baudrate, timeout = 0.1)

        _stop_at = time.time () + timeout
        while self.serial.readline ().strip () != 'READY':
            if time.time () >= _stop_at:
                raise IOError ('Connection timeout')

        self.serial.write ('config\r')
        while self.serial.readline ().strip () != self.osd_prompt:
            pass

        self.connected = True

        self.info ()
        self.eeprom = self.eeprom_read ()
        self.options = catalog.options [self.version].copy ()
        self.options.board = self
        self.options.load ()

    def disconnect (self):
        if not self.connected:
            return
        self.serial.close ()
        self._reset ()

    @_connection
    def execute (self, cmd):
        self.serial.write ('%s\r' % cmd)
        echo = self.serial.readline ().strip ()
        if echo != cmd:
            raise IOError ('Invalid board response')

    @_connection
    def info (self):
        self.execute ('info')
        panels = False
        for line in iter (self.serial.readline (), self.osd_prompt):
            if not line:
                continue

            if panels:
                self.panels.append (line.split (':')[1].strip ())
                continue

            m = self.info_regexp.match (line)
            if not m:
                raise IOError ('Unknown info response')
            if m.group (1) == 'VERSION':
                self.version = m.group (2)
            elif m.group (1) == 'MODULES':
                self.modules = m.group (2).split (' ')
            elif m.group (1) == 'PANELS':
                panels = True

    @_connection
    def eeprom_read (self):
        self.execute ('eeprom r')
        res = self.serial.read (const.EEPROM_SIZE)
        self.serial.readline ()
        self.serial.readline ()
        return bytearray (res)

    @_connection
    def eeprom_write (self, data):
        self.execute ('eeprom w')
        self.serial.write (bytes (data [:const.EEPROM_SIZE]))
        self.serial.readline ()
        self.serial.readline ()
        self.serial.readline ()

    def __repr__ (self):
        return '<Board port={} connected={}, version={}, modules={}, panels={}>'.format (
            self.port, self.connected, self.version, self.modules, self.panels
        )

