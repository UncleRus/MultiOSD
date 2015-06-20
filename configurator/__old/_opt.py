#!/usr/bin/env python
# -*- coding: utf-8 -*-

import json
import struct
import re
from serial import Serial
import time
from PySide.QtCore import QObject, Signal

from pprint import pprint
import os


class Setting (object):

    settings = None
    _types = {
        bool: lambda x: x.lower () in ('true', 'yes', '1') if isinstance (x, basestring) else x,
        dict: lambda x: ast.literal_eval (x)
    }

    @classmethod
    def init (cls):
        if not cls.settings:
            cls.settings = QSettings ()

    def __init__ (self, key = None, default = None, type_ = None):
        self.init ()
        self.key = key
        self.default = default
        self.type = type_ or type (default)
        if not self.type:
            raise TypeError ('Undefined option type')

    def __get__ (self, owner, cls):
        return self._types.get (self.type, self.type) (self.settings.value (self.key, self.default))

    def __set__ (self, owner, value):
        self.settings.setValue (self.key, value)




class QBoard (QObject):

    timeoutOccured = Signal ()
    connectionChanged = Signal (bool)
    errorOccured = Signal (str)
    stateChanged = Signal ([str, int])

    def __init__ (self, maps_dir):
        self.maps_dir = maps_dir
        self.eeprom = None
        self._options = OptionsMap ()
        self.board = Board ('/dev/ttyUSB0' if os.name == 'posix' else 'COM1', timeout = 10)
        self._version, self._modules, self._panels = None

    def options (self):
        return self._options

    def version (self):
        return self._version

    def modules (self):
        return self._modules

    def panels (self):
        return self._panels

    def isConnected (self):
        return self.board.connected

    def connect (self):
        if self.isConnected ():
            self.disconnect ()
        try:
            # TODO: connection timeout in settings
            # connecting
            self.stateChanged.emit ('connected', 0)
            self.board.connect (timeout = 5)
            self.connectionChanged.emit (True)
            self.stateChanged.emit ('connected', 25)
            # info
            self._version, self._modules, self._panels = self.board.info ()
            self.stateChanged.emit ('connected', 50)
            # eeprom
            self.eeprom = self.board.eeprom_read ()
            self._options.parse (json.loads (open (os.path.join (self.maps_dir, '%map.json'), 'rb').read ()), encoding = 'utf-8')
            self._options.load (self.eeprom)
            self.stateChanged.emit ('connected', 100)
        except TimeoutError:
            self.stateChanged.emit ('timeout', 0)
            self.timeoutOccured.emit ()
        except Exception as e:
            self.stateChanged.emit ('error', 0)
            self.errorOccured.emit (repr (e))

    def saveOptions (self):
        try:
            eeprom = self._options.save ()
            # FIXME: экраны
            self.stateChanged.emit ('save', 0)
            self.board.eeprom_write (self.eeprom)
            self.stateChanged.emit ('save', 50)
            self.eeprom = self.board.eeprom_read ()
            if self.eeprom != eeprom:
                raise IOError ('EEPROM write error')
            self.stateChanged.emit ('save', 100)
        except Exception as e:
            self.stateChanged.emit ('error', 0)
            self.errorOccured.emit (repr (e))

    def fontCallback (self, state, i):
        if (i - 1) % 64 == 0:
            self.stateChanged.emit (state, i)

    def uploadFont (self, filename):
        try:
            self.board.font_upload (open (filename, 'rb').read (), lambda i: self.fontCallback ('font_upload', i))
        except Exception as e:
            self.errorOccured.emit (repr (e))

    def downloadFont (self, filename):
        try:
            self.board.font_download (open (filename, 'rb').read (), lambda i: self.fontCallback ('font_download', i))
        except Exception as e:
            self.errorOccured.emit (repr (e))

    def disconnect (self):
        if self.isConnected ():
            try:
                self.board.disconnect ()
                self.connectionChanged.emit (False)
                self.stateChanged.emit ('disconnected', 0)
            except Exception as e:
                self.errorOccured.emit (repr (e))

    def port (self):
        return self.board.port

    def setPort (self, value):
        if self.board.port == value:
            return
        if self.isConnected ():
            self.disconnect ()
        self.board.port = value

    def baudrate (self):
        return self.board.baudrate

    def setBaudrate (self, value):
        if self.board.baudrate == value:
            return
        if self.isConnected ():
            self.disconnect ()
        self.board.baudrate = value



m = OptionsMap (open ('0005map.json', 'rb').read ())
pprint (m.sections)

