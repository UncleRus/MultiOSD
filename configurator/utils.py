# -*- coding: utf-8 -*-

import ast
from PySide.QtCore import QSettings


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
