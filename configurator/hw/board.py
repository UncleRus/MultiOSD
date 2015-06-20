# -*- coding: utf-8 -*-

import re
import time
from serial import Serial


__all__ = ['TimeoutError', 'Board']


def needs_connection (func):
    def _wrapped (*args, **kwargs):
        if not args [0].connected:
            raise IOError ('Board not found')
        return func (*args, **kwargs)
    return _wrapped


class TimeoutError (IOError):
    pass


class Board (object):

    EEPROM_SIZE = 0x400

    osd_prompt = 'osd#'

    info_regexp = re.compile (r'([A-Z]+):\s*(.*)?')

    def __init__ (self, port = '/dev/ttyUSB0', baudrate = 57600):
        self.port = port
        self.baudrate = baudrate
        self.connected = False

    def connect (self, timeout = 5):
        if self.connected:
            self.disconnect ()

        self.serial = Serial (self.port, self.baudrate, timeout = 0.1)

        _stop_at = time.time () + timeout
        while self.serial.readline ().strip () != 'READY':
            if time.time () >= _stop_at:
                raise TimeoutError ('Connection timeout')

        self.serial.write (b'config\r')
        while self.serial.readline ().strip () != self.osd_prompt:
            pass

        self.connected = True
        self.info ()

    def disconnect (self):
        if not self.connected:
            return
        self.serial.close ()
        self.connected = False

    @needs_connection
    def execute (self, cmd):
        self.serial.write (b'%s\r' % cmd)
        echo = self.serial.readline ().strip ()
        if echo != cmd:
            raise IOError ('Invalid board response')

    @needs_connection
    def info (self):
        version = 0
        modules = []
        panels = []

        self.execute ('info')

        _pan = False
        for line in iter (lambda: self.serial.readline ().strip (), self.osd_prompt):
            if not line:
                continue

            if _pan:
                panels.append (line.split (':')[1].strip ())
                continue

            m = self.info_regexp.match (line)
            if not m:
                raise IOError ('Unknown info response')
            if m.group (1) == 'VERSION':
                version = m.group (2)
            elif m.group (1) == 'MODULES':
                modules = m.group (2).split (' ')
            elif m.group (1) == 'PANELS':
                _pan = True

        return (version, modules, panels)

    @needs_connection
    def eeprom_read (self):
        self.execute ('eeprom r')
        res = self.serial.read (self.EEPROM_SIZE)
        self.serial.readline ()
        self.serial.readline ()
        return bytearray (res)

    @needs_connection
    def eeprom_write (self, data):
        self.execute ('eeprom w')
        self.serial.write (bytes (data [:self.EEPROM_SIZE]))
        self.serial.readline ()
        self.serial.readline ()
        self.serial.readline ()

    @needs_connection
    def font_upload (self, fp, callback = None):
        self.execute ('font u')
        for i in xrange (16385):
            line = fp.readline ()
            if not line:
                raise IOError ('Invalid file format')
            self.serial.write (line)
            if callback:
                callback (i)
        self.serial.write (b'\r\r')

    @needs_connection
    def font_download (self, fp, callback = None):
        self.execute ('font d')
        for i in xrange (16385):
            line = self.serial.readline ().strip ()
            if line == self.osd_prompt:
                raise IOError ('Cannot download font')
            fp.write (line)
            fp.write (b'\r\n')
            if callback:
                callback (i)

    def __repr__ (self):
        return '<Board port={} connected={}, version={}, modules={}, panels={}>'.format (
            self.port, self.connected, self.version, self.modules, self.panels
        )
