# -*- coding: utf-8 -*-

import struct
from . import const


__all__ = ['BoolOption', 'FloatOption', 'UInt8Option', 'UInt16Option', 'StrOption', 'EnumOption', 'Section', 'Options']


class Option (object):

    def __init__ (self, name, offset, description, fmt, readonly = False):
        self.name = name
        self.offset = offset
        self.fmt = fmt
        self.description = description
        self.section = None
        self.value = None
        self.readonly = readonly

    def copy (self):
        return type (self) (self.name, self.offset, self.description, readonly = self.readonly)

    def get (self):
        res = struct.unpack_from (self.fmt, self.section.eeprom (), self.offset)
        self.value = res [0] if len (res) == 1 else res
        return self.value

    def pack (self):
        return struct.pack (self.fmt, self.value)

    def __repr__ (self):
        return '%s=%r' % (self.name, self.get ())


class BoolOption (Option):

    def __init__ (self, name, offset, description, **kwargs):
        super (BoolOption, self).__init__ (name, offset, description, 'B', **kwargs)

    def get (self):
        return bool (super (BoolOption, self).get ())


class FloatOption (Option):

    def __init__ (self, name, offset, description, **kwargs):
        super (FloatOption, self).__init__ (name, offset, description, '<f', **kwargs)


class UInt8Option (Option):

    def __init__ (self, name, offset, description, **kwargs):
        super (UInt8Option, self).__init__ (name, offset, description, 'B', **kwargs)


class UInt16Option (Option):

    def __init__ (self, name, offset, description, **kwargs):
        super (UInt16Option, self).__init__ (name, offset, description, '<H', **kwargs)


class StrOption (Option):

    def __init__(self, name, offset, description, length, **kwargs):
        super (StrOption, self).__init__ (name, offset, description, '%ds' % length, **kwargs)
        self.length = length

    def copy (self):
        return type (self) (self.name, self.offset, self.description, self.length, readonly = self.readonly)


class EnumOption (Option):

    def __init__ (self, name, offset, description, items = (), fmt = 'B', **kwargs):
        super (EnumOption, self).__init__ (name, offset, description, fmt, **kwargs)
        self.items = items

    def copy (self):
        return type (self) (self.name, self.offset, self.description, self.items, self.fmt, readonly = self.readonly)

    def get (self):
        idx = super (EnumOption, self).get ()
        if isinstance (self.items, dict):
            if idx in self.items:
                return idx, self.items [idx]
            else:
                return self.items.items ()[0]
        idx = idx if 0 <= idx < len (self.items) else 0
        return idx, self.items [idx]

    def __iter__ (self):
        return ((idx, item) for idx, item in (self.items.items () if isinstance (self.items, dict) else enumerate (self.items)))


class Iterable (object):

    def __init__ (self, names):
        self.names = names

    def __getitem__ (self, name):
        return self.names [name]

    def __iter__ (self):
        return self.names.__iter__


class Section (Iterable):

    def __init__ (self, name, offset, *options, **kwargs):
        super (Section, self).__init__ ({opt.name: opt for opt in options})
        self.name = name
        self.offset = offset
        self.options = options
        for opt in options:
            opt.section = self
        self.board = None
        self.persistent = kwargs.get ('persistent', True)
        self.enabled = True

    def eeprom (self):
        return self.board.eeprom [self.offset:]

    def build (self, res):
        for opt in self.options:
            data = tuple (opt.pack ())
            res [self.offset + opt.offset : self.offset + opt.offset + len (data)] = data

    def load (self):
        for opt in self.options:
            opt.get ()

    def copy (self):
        return Section (self.name, self.offset, *[option.copy () for option in self.options], persistent = self.persistent)

    def __repr__ (self):
        res = ['[%s]' % self.name]
        for opt in self.options:
            res.append (repr (opt))
        return '\n'.join (res)


class Options (Iterable):

    def __init__ (self, *sections):
        super (Options, self).__init__ ({section.name: section for section in sections})
        self._board = None
        self.sections = sections

    @property
    def board (self):
        return self._board

    @board.setter
    def board (self, value):
        self._board = value
        for section in self.sections:
            section.board = self._board
            section.enabled = self._board and (section.persistent or section.name in self._board.modules)

    def build (self):
        res = bytearray ([0xff] * const.EEPROM_SIZE)
        for section in self.sections:
            if section.enabled:
                section.build (res)
        return res

    def save (self):
        self._board.eeprom_write (self.build ())

    def load (self):
        for section in self.sections:
            section.load ()

    def copy (self):
        return Options (*[section.copy () for section in self.sections])

    def __repr__ (self):
        return '\n\n'.join ((repr (section) for section in self.sections))

