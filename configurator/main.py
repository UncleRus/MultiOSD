#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PySide.QtGui import QWidget, QHBoxLayout, QLabel
from board import QBoard
from ui import Pager
from controls import PageWidget


class MainWindow (QWidget):

    pages = (
        ('Hardware', None),
        ('Picture', None),
        ('Battery', None),
        ('Telemetry', None),
        ('OSD', None),
        ('Screens', None),
    )

    def __init__ (self):
        super (MainWindow, self).__init__ ()
        self.lMain = QHBoxLayout (self)
        self.pager = Pager (self)
        pages = (
            ('Hardware', QLabel ('<h1>Test</h1>')),
            ('Picture', PageWidget ('Picture',)),
            ('Battery', None),
            ('Telemetry', None),
            ('OSD', None),
            #('Screens', None),
        )


if __name__ == '__main__':
    pass
