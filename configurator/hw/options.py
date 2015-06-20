# -*- coding: utf-8 -*-

import struct
from .board import Board


__all__ = ['Option', 'StrOption', 'EnumOption', 'NumericOption', 'Section', 'OptionsMap']


class Option (object):

    @classmethod
    def create (cls, name, **params):
        return _opt_types [params.get ('type', 'uint8')] (name, **params)

    def __init__ (self, name, **kwargs):
        self.name = name
        self.offset = kwargs ['offset']
        self.description = kwargs ['descr']
        self.type = kwargs ['type']
        self.readonly = kwargs.get ('readonly', False)
        self.value = None

    def pack (self):
        return self.struct.pack (self.value)

    def pack_into (self, addr, data):
        packed = self.pack ()
        data [addr + self.offset : addr + self.offset + len (packed)] = packed

    def unpack (self, addr, data):
        self.value = self.struct.unpack_from (data, addr + self.offset)
        if len (self.value) == 1:
            self.value = self.value [0]


class StrOption (Option):

    def __init__ (self, name, **kwargs):
        super (StrOption, self).__init__ (name, **kwargs)
        self.length = kwargs ['length']
        self.struct = struct.Struct ('%ds' % self.length)


class EnumOption (Option):

    def __init__ (self, name, **kwargs):
        super (EnumOption, self).__init__ (name, **kwargs)
        self.items = kwargs.get ['items']
        self.struct = struct.Struct ('B')
        if not isinstance (self.items, dict):
            self.items = dict (enumerate (self.items))


def NumericOption (bfmt):

    class _NumericOption (Option):

        def __init__ (self, name, **kwargs):
            super (_NumericOption, self).__init__ (name, **kwargs)
            self.struct = struct.Struct (bfmt)
            self.min = kwargs.get ('min')
            self.max = kwargs.get ('max')

    return _NumericOption


_opt_types = {
    'uint8': NumericOption ('B'),
    'uint16': NumericOption ('<H'),
    'bool': NumericOption ('B'),
    'float': NumericOption ('f'),
    'enum': NumericOption ('B'),
    'str': StrOption
}

class Section (dict):

    def __init__ (self, name, **kwargs):
        super (Section, self).__init__ ()
        self.name = name
        self.address = kwargs ['address']
        self.persistent = kwargs ['persistent']
        self.enabled = True

    def pack_into (self, data):
        for opt in self.values ():
            if not opt.readonly:
                opt.pack_into (self.address, data)

    def unpack (self, data):
        for opt in self.values ():
            opt.unpack (self.address)


class OptionsMap (object):

    def __init__ (self, struct = None):
        self.sections = {}
        self.map = {}
        if struct:
            self.parse (struct)

    def parse (self, struct):
        self.sections.clear ()
        for sname, sdef in struct ['options'].items ():
            self.sections [sname] = Section (sname, **sdef)
            for oname, odef in sdef ['options'].items ():
                opt = Option.create (oname, **odef)
                self.sections [sname][oname] = opt
                self.map ['%s/%s' % (sname, oname)] = opt

    def load (self, eeprom):
        for s in self.sections:
            s.unpack (eeprom)

    def save (self, modules):
        result = bytearray ([0xff] * Board.EEPROM_SIZE)
        for s in self.sections:
            if s.persistent or s.name in modules:
                s.pack_into (result)
        return result
