#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
import os
from PySide.QtCore import *
from PySide.QtGui import *
import ast


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


class Pager (QWidget):

#    s_port = Setting ('main/port', 'COM1' if os.name == 'nt' else '/dev/ttyUSB0')
#    s_baudrate = Setting ('main/baudrate', 57600)

    class SectionButton (QToolButton):

        def __init__ (self, text, widget, parent = None):
            super (Pager.SectionButton, self).__init__ (parent)
            self.widget = widget
            self.setIconSize (QSize (48, 48))
            self.setSizePolicy (QSizePolicy (QSizePolicy.Fixed, QSizePolicy.Fixed))
            self.setMinimumSize (QSize (80, 80))
            self.setMaximumSize (self.minimumSize ())
            self.setToolButtonStyle (Qt.ToolButtonTextUnderIcon)
            self.setText (text)
            self.setIcon (QIcon ('icons/%s.png' % text))
            self.setCheckable (True)

    class Ui (QObject):

        def __init__ (self, owner):
            super (Pager.Ui, self).__init__ (owner)
            self.owner = owner
            self.lMain = QHBoxLayout (owner)

            # Left pane for buttons
            self.bPanel = QFrame (owner)
            self.bPanel.setFrameStyle (QFrame.Sunken + QFrame.StyledPanel)
            self.bPanel.setSizePolicy (QSizePolicy (QSizePolicy.Fixed, QSizePolicy.MinimumExpanding))
            self.lBPanel = QVBoxLayout (self.bPanel)
            self.lMain.addWidget (self.bPanel)

            self.container = QFrame (owner)
            self.container.setFrameStyle (QFrame.Sunken + QFrame.StyledPanel)
            self.container.setSizePolicy (QSizePolicy (QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding))
            self.lMain.addWidget (self.container)

            self.lContainer = QVBoxLayout (self.container)

            self.buttons = []
            self.pages = {}
            for name, page in owner.pages:
                btn = Pager.SectionButton (name, page, self.bPanel)
                self.lBPanel.addWidget (btn)
                self.buttons.append (btn)
                btn.clicked.connect (lambda sender = btn: self.onButtonClicked (sender))
                self.pages [btn] = page
                self.lContainer.addWidget (page)
                page.hide ()
            self.lBPanel.addStretch (1)
            self.lContainer.addStretch ()

#            if self.buttons:
#                self.buttons [0].clicked.emit ()

        def onButtonClicked (self, sender):
            for btn, page in ((_, self.pages [_]) for _ in self.buttons):
                found = btn == sender
                btn.setChecked (found)
                page.setVisible (found)

    def __init__ (self, parent = None):
        super (Pager, self).__init__ (parent)
        self.pages = (
            ('Hardware', QLabel ('<h1>Hardware</h1>')),
            ('Picture', QLabel ('<h1>Picture</h1>')),
            ('Battery', QLabel ('<h1>Battery</h1>')),
            ('Location', QLabel ('<h1>Location</h1>')),
            ('Screens', QLabel ('<h1>Screens</h1>')),
            ('Other', QLabel ('<h1>Other</h1>')),
        )
        self.ui = self.Ui (self)


#app = QApplication (sys.argv)
#
#w = Pager ()
#w.setEnabled (False)
#w.show ()
#
#sys.exit (app.exec_ ())


