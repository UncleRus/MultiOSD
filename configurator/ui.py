# -*- coding: utf-8 -*-

from PySide.QtCore import QSize, Qt, QObject
from PySide.QtGui import QWidget, QToolButton, QSizePolicy, QHBoxLayout, QFrame, QIcon, QVBoxLayout


class Pager (QWidget):

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
            self.setIcon (QIcon ('res/icons/%s.png' % text))
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
            self.lOuterButtons = QVBoxLayout (self.bPanel)
            self.lButtons = QVBoxLayout ()
            self.lOuterButtons.addLayout (self.lButtons)
            self.lOuterButtons.addStretch (1)
            self.lMain.addWidget (self.bPanel)

            self.container = QFrame (owner)
            self.container.setFrameStyle (QFrame.Sunken + QFrame.StyledPanel)
            self.container.setSizePolicy (QSizePolicy (QSizePolicy.MinimumExpanding, QSizePolicy.MinimumExpanding))
            self.lMain.addWidget (self.container)

            self.lOuterContainer = QVBoxLayout (self.container)
            self.lContainer = QVBoxLayout ()
            self.lOuterContainer.addLayout (self.lContainer)
            self.lOuterContainer.addStretch ()

    def __init__ (self, parent = None):
        super (Pager, self).__init__ (parent)
        self.pages = {}
        self.ui = self.Ui (self)

    def addPage (self, name, page):
        if name in self.pages:
            raise ValueError ('Duplicate page name')
        button = Pager.SectionButton (name, page, self.ui.bPanel)
        button.clicked.connect (lambda page_name = name: self.ui.onButtonClicked (page_name))
        self.ui.lButtons.addWidget (button)
        self.ui.lContainer.addWidget (page)
        page.hide ()
        button.setEnabled (page.isEnabled ())
        self.pages [name] = (button, page)

    def onButtonClicked (self, page_name):
        for name, (button, page) in self.pages.items ():
            button.setChecked (name == page_name)
            page.setVisible (name == page_name)

    def setPageEnabled (self, page_name, value):
        button, page = self.pages [page_name]
        button.setEnabled (value)
        page.setEnabled (value)



