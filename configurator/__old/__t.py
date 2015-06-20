#!/usr/bin/env python
# -*- coding: utf-8 -*-

import sys
from PySide.QtCore import *
from PySide.QtGui import *

import hw

class Control (QWidget):

    changed = Signal ()

    def __init__ (self, option, owner = None, setup = True):
        super (Control, self).__init__ (owner)
        self.option = option
        if setup:
            self.setupUi ()

    def setupUi (self):
        self.input.setDisabled (self.option.readonly)


class BoolControl (Control):

    def setupUi (self):
        self.label = None
        self.input = QCheckBox (self.option.name, self)
        self.input.setToolTip (self.option.description)
        self.input.stateChanged.connect (lambda _: self.changed.emit ())
        super (BoolControl, self).setupUi ()

    def load (self):
        self.input.setChecked (self.option.value)

    def save (self):
        self.option.value = self.input.isChecked ()


class FloatControl (Control):

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QDoubleSpinBox (self)
        self.input.setSingleStep (0.1)
        self.input.setToolTip (self.option.description)
        self.input.valueChanged.connect (lambda _: self.changed.emit ())
        super (FloatControl, self).setupUi ()

    def load (self):
        print self.option
        self.input.setValue (self.option.value)

    def save (self):
        self.option.value = self.input.value ()


class EnumControl (Control):

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QComboBox (self)
        self.index = {}
        for i, (idx, item) in enumerate (self.option):
            self.input.addItem (item, idx)
            self.index [idx] = i
        self.input.setToolTip (self.option.description)
        self.input.currentIndexChanged.connect (lambda _: self.changed.emit ())
        super (EnumControl, self).setupUi ()

    def load (self):
        print self.option
        try:
            self.input.setCurrentIndex (self.index [self.option.value])
        except:
            self.input.setCurrentIndex (0)

    def save (self):
        self.option.value = self.input.itemData (self.input.currentIndex ())


class IntControl (Control):

    def __init__ (self, option, min, max, owner = None):
        super (IntControl, self).__init__ (option, owner, setup = False)
        self.min = min
        self.max = max
        self.setupUi ()

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QSpinBox (self)
        self.input.setMinimum (self.min)
        self.input.setMaximum (self.max)
        self.input.setToolTip (self.option.description)
        self.input.valueChanged.connect (lambda _: self.changed.emit ())
        super (IntControl, self).setupUi ()

    def load (self):
        print self.option
        self.input.setValue (self.option.value)

    def save (self):
        self.option.value = self.input.value ()


class UInt16Control (IntControl):

    def __init__ (self, option, owner = None):
        super (UInt16Control, self).__init__ (option, 0, 0xffff, owner = owner)


class UInt8Control (IntControl):

    def __init__ (self, option, owner = None):
        super (UInt8Control, self).__init__ (option, 0, 0xff, owner = owner)


class StrControl (Control):

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QLineEdit (self)
        self.input.setMaxLength (self.option.length)
        self.input.setToolTip (self.option.description)
        self.input.editingFinished.connect (lambda _: self.changed.emit ())
        super (StrControl, self).setupUi ()

    def load (self):
        print self.option
        self.input.setText (self.option.value)

    def save (self):
        self.option.value = self.input.text ()


class PageWidget (QWidget):

    _factories = {
        'bool': BoolControl,
        'float': FloatControl,
        'enum': EnumControl,
        'uint8': UInt8Control,
        'uint16': UInt16Control,
        'str': StrControl
    }

    def __init__ (self, section, owner = None):
        super (PageWidget, self).__init__ (owner)
        self.setSizePolicy (QSizePolicy (QSizePolicy.Minimum, QSizePolicy.Fixed))
        self.fl = None
        self.clear ()
        self.setSection (section)

    def clear (self):
        if self.fl:
            self.fl.deleteLater ()
        self._section = None
        self.setTitle (u'')
        self.fl = QFormLayout (self)
        self.fields = {}

    def load (self):
        if not self._section.enabled:
            return
        for field in self.fields.values ():
            field.load ()

    def save (self):
        if not self._section.enabled:
            return
        for field in self.fields.values ():
            if not field.option.readonly:
                field.save ()

    def setSection (self, value):
        if self._section:
            self.clear ()
        self._section = value
        if not self._section:
            return
        for option in self._section.options:
            field = self._factories [type (option)] (option, self)
            self.fields [option.name] = field
            self.fl.addRow (field.label, field.input)
        self.setEnabled (self._section.enabled)
        self.setTitle (self._section.name)


class OptionsWidget (QWidget):

    def __init__ (self, options, modules, owner = None):
        super (OptionsWidget, self).__init__ (owner)
        self.items = []
        self.l_outer = QVBoxLayout (self)
        self.box = QScrollArea (self)
        self.box.setWidgetResizable (True)
        self.l_outer.addWidget (self.box)
        self.l_main = QVBoxLayout (self.box)
        self.clear ()
        self.setOptions (options)
        self.setMinimumWidth (300)
        self.setMinimumHeight (400)
        self.modules = modules

    def setOptions (self, value):
        if self._options:
            self.clear ()
        self._options = value
        if not self._options:
            return
        for sect in self._options.sections:
            if sect.name in ('HEADER', 'SCREENS'):
                continue
            self.l_main.addWidget (SectionWidget (sect, self))

    def clear (self):
        for item in self.items:
            item.deleteLater ()
        self.items = []
        self._options = None

    def iterwidgets (self):
        return iter (self.items)

    def save (self):
        for widget in self.iterwidgets ():
            widget.save ()

    def load (self):
        for widget in self.iterwidgets ():
            widget.load ()


app = QApplication (sys.argv)

b = hw.board.Board ()
print 'RESET'
b.connect ()
print b.version, b.modules, b.panels
w = QWidget ()
w.l = QVBoxLayout (w)
opt = OptionsWidget (b.options, b.modules)
w.l.addWidget (opt)
opt.load ()
#print '%r' % bytes (b.options.build ())
print '%r' % b.options.build ()
w.show ()

sys.exit (app.exec_ ())
