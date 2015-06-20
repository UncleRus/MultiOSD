# -*- coding: utf-8 -*-

from PySide.QtCore import Signal
from PySide.QtGui import QWidget, QCheckBox, QLabel, QDoubleSpinBox, \
    QComboBox, QSpinBox, QFormLayout, QLineEdit, QSizePolicy


class Control (QWidget):

    changed = Signal ()

    def __init__ (self, option, owner = None):
        super (Control, self).__init__ (owner)
        self.option = option
        self.setupUi ()

    def setupUi (self):
        self.input.setDisabled (self.option.readonly)

    def onChanged (self):
        self.changed.emit ()


class BoolControl (Control):

    def setupUi (self):
        self.label = None
        self.input = QCheckBox (self.option.name, self)
        self.input.setToolTip (self.option.description)
        self.input.stateChanged.connect (self.onChanged)
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
        self.input.valueChanged.connect (self.onChanged)
        super (FloatControl, self).setupUi ()

    def load (self):
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
        self.input.currentIndexChanged.connect (self.onChanged)
        super (EnumControl, self).setupUi ()

    def load (self):
        try:
            self.input.setCurrentIndex (self.index [self.option.value])
        except:
            self.input.setCurrentIndex (0)

    def save (self):
        self.option.value = self.input.itemData (self.input.currentIndex ())


class IntControl (Control):

    def __init__ (self, option, owner = None):
        super (IntControl, self).__init__ (option, owner)
        self.setupUi ()

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QSpinBox (self)
        self.input.setMinimum (self.min ())
        self.input.setMaximum (self.max ())
        self.input.setToolTip (self.option.description)
        self.input.valueChanged.connect (self.onChanged)
        super (IntControl, self).setupUi ()

    _bounds = {
        'uint8': (0x00, 0xff),
        'uint16': (0x00, 0xff),
    }

    def min (self):
        if self.option.min is not None:
            return self.option.min
        return self._bounds [self.option.type][0]

    def max (self):
        if self.option.max is not None:
            return self.option.max
        return self._bounds [self.option.type][1]

    def load (self):
        self.input.setValue (self.option.value)

    def save (self):
        self.option.value = self.input.value ()


class StrControl (Control):

    def setupUi (self):
        self.label = QLabel (self.option.name, self)
        self.input = QLineEdit (self)
        self.input.setMaxLength (self.option.length)
        self.input.setToolTip (self.option.description)
        self.input.editingFinished.connect (lambda _: self.changed.emit ())
        super (StrControl, self).setupUi ()

    def load (self):
        self.input.setText (self.option.value)

    def save (self):
        self.option.value = self.input.text ()


class PageWidget (QWidget):

    _factories = {
        'bool': BoolControl,
        'float': FloatControl,
        'enum': EnumControl,
        'uint8': IntControl,
        'uint16': IntControl,
        'str': StrControl
    }

    changed = Signal ()

    def __init__ (self, page, board, parent = None):
        super (PageWidget, self).__init__ (parent)
        self.board = board

        self.setSizePolicy (QSizePolicy (QSizePolicy.Maximum, QSizePolicy.Maximum))
        self.lMain = QFormLayout (self)

        self.controls = []
        for optname in page:
            opt = self.board.options.map [optname]
            ctrl = self._factories [opt.type] (opt, self)
            ctrl.changed.connect (self.onChanged)
            self.controls.append (ctrl)
            self.lMain.addRow (ctrl.label, ctrl.input)

    def save (self):
        for ctrl in self.controls:
            ctrl.save ()

    def load (self):
        for ctrl in self.controls:
            ctrl.load ()

    def onChanged (self):
        self.changed.emit ()


