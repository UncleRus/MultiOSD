#!/usr/bin/env python
# -*- coding: utf-8 -*-

from PySide.QtCore import *
from PySide.QtGui import *
import sys


CHAR_WIDTH = 12
CHAR_HEIGHT = 18


class Char (QImage):

    colors = (
        qRgb (0, 0, 0),
        qRgba (128, 128, 128, 0),
        qRgb (255, 255, 255),
        qRgba (128, 128, 128, 0),
    )

    def __init__ (self, data):
        super (Char, self).__init__ (CHAR_WIDTH, CHAR_HEIGHT, QImage.Format.Format_Indexed8)
        for i, c in enumerate (self.colors):
            self.setColor (i, c)

        for r in xrange (CHAR_HEIGHT):
            for c in xrange (CHAR_WIDTH / 4):
                byte = data [r * 3 + c]
                for s in xrange (3, -1, -1):
                    color = (byte >> (s * 2)) & 0x03
                    x = c * 4 + 3 - s
                    self.setPixel (x, r, color)


class Font (object):

    def __init__ (self, file):
        self.load (file)

    def load (self, file):
        if file.readline ().strip () != 'MAX7456':
            raise IOError ('Unknown file format')
        data = [int (file.readline ().strip (), 2) for _ in xrange (16384)]
        self.chars = [Char (data [i * 64 : i * 64 + 64]) for i in xrange (256)]

    def write (self, textbuf, dest = None):
        if isinstance (textbuf, (bytes, bytearray)):
            textbuf = (textbuf,)

        if not dest:
            maxlen = max ((len (line) for line in textbuf))
            dest = QImage (QSize (maxlen * CHAR_WIDTH, len (textbuf) * CHAR_HEIGHT), QImage.Format_ARGB32_Premultiplied)
            dest.fill (qRgba (0, 0, 0, 0))

        painter = QPainter (dest)
        pos = QPoint (0, 0)
        for line in textbuf:
            pos.setX (0)
            for c in bytearray (line):
                painter.drawImage (pos, self.chars [c])
                pos.setX (pos.x () + CHAR_WIDTH)
            pos.setY (pos.y () + CHAR_HEIGHT)
        painter.end ()

        return dest


class PanelType (object):

    def __init__ (self, name, size, textbuf, minSize = None):
        self.name = name
        self.width, self.height = size
        self.textbuf = textbuf
        self.minWidth, self.minHeight = minSize or size

    def rect (self, offset):
        return QRect (0, 0, self.width, self.height).translated (offset)


class Panel (QGraphicsItem):

    def __init__ (self, scene, fixedFont, panelType):
        super (Panel, self).__init__ ()
        self.overlapped = False
        self.setFlag (QGraphicsItem.ItemIsSelectable, True)
        self.fixedFont = None
        self.panelType = None
        self.dragging = False
        self.setFixedFont (fixedFont)
        self.setPanelType (panelType)
        scene.addItem (self)

    def setFixedFont (self, value):
        self.fixedFont = value
        self.updatePixmap ()

    def setPanelType (self, value):
        self.panelType = value
        self.updatePixmap ()

    def updatePixmap (self):
        if self.fixedFont and self.panelType:
            self._rect = QRectF (0, 0, self.panelType.width * CHAR_WIDTH, self.panelType.height * CHAR_HEIGHT)
            self.pixmap = QPixmap.fromImage (self.fixedFont.write (self.panelType.textbuf))
            self.boundingRect()

    def screenPos (self):
        return self.scene ().toScreenPos (self.pos ())

    def setScreenPos (self, pos):
        self.setPos (self.scene ().toScenePos (pos))

    def boundingRect (self):
        return self._rect

    def paint (self, painter, option, widget):
        painter.drawPixmap (0, 0, self.pixmap)
        if self.overlapped:
            painter.setPen (QPen (Qt.red, 0, Qt.SolidLine))
            painter.drawRect (self._rect)
        if self.isSelected ():
            painter.setPen (QPen (Qt.white, 0, Qt.DashLine))
            painter.drawRect (self._rect)

    def mousePressEvent (self, e):
        super (Panel, self).mousePressEvent (e)
        if e.button () == Qt.LeftButton:
            self.dragging = True
            self.dragStartPos = self.scene ().position ()

    def mouseReleaseEvent (self, e):
        super (Panel, self).mouseReleaseEvent (e)
        self.dragging = False

    def mouseMoveEvent (self, e):
        if not self.dragging or not self.scene ():
            return
        pos = self.pos () + self.scene ().position () - self.dragStartPos
        self.dragStartPos = self.scene ().position ()
        self.scene ().movePanel (self, pos)
        self.scene ().invalidate ()

    def itemChange (self, change, value):
        if self.scene ():
            if change == self.ItemSceneHasChanged:
                self.updateOverlapped ()
            elif change == self.ItemSelectedHasChanged:
                self.scene ().invalidate ()
        return super (Panel, self).itemChange (change, value)

    def updateOverlapped (self):
        scrPos = self.screenPos ()
        scrRect = self.panelType.rect (scrPos)
        for panel in self.scene ().items ():
            if panel == self:
                continue
            self.overlapped = scrRect.intersects (panel.panelType.rect (panel.screenPos ()))
            if self.overlapped:
                break
        if not self.overlapped:
            ovlX = self.scene ().cols - self.panelType.width
            ovlY = self.scene ().rows - self.panelType.height
            self.overlapped = scrPos.x () > ovlX or scrPos.y () > ovlY
        self.update ()


class Screen (QGraphicsScene):

    def __init__ (self, cols, rows, fixedFont, bgImage = None, owner = None):
        super (Screen, self).__init__ (owner)
        self.setScreenSize (cols, rows)
        self.setBgImage (bgImage)
        self.fixedFont = fixedFont
        self._screenPos = QPoint (0, 0)
        self._position = QPoint (0, 0)

    def setScreenSize (self, cols, rows):
        self.cols = cols
        self.rows = rows
        self.setSceneRect (0, 0, self.cols * CHAR_WIDTH, self.rows * CHAR_HEIGHT)

    def setBgImage (self, image):
        self.bgImage = image
        rect = self.sceneRect ().toRect ()
        bgBuf = QImage (rect.size (), QImage.Format_ARGB32_Premultiplied)
        painter = QPainter (bgBuf)
        if image:
            painter.drawImage (rect, image.scaled (rect.size (), Qt.IgnoreAspectRatio, Qt.SmoothTransformation))
        else:
            painter.fillRect (rect, Qt.gray)
        painter.end ()
        self.bgPixmap = QPixmap.fromImage (bgBuf)
        self.update ()

    def closestPos (self, point):
        return QPoint (
            round (point.x () / CHAR_WIDTH) * CHAR_WIDTH,
            round (point.y () / CHAR_HEIGHT) * CHAR_HEIGHT
        )

    def toScreenPos (self, point):
        return QPoint (
            round (point.x () / CHAR_WIDTH),
            round (point.y () / CHAR_HEIGHT)
        )

    def toScenePos (self, point):
        return QPoint (point.x () * CHAR_WIDTH, point.y () * CHAR_HEIGHT)

    def position (self):
        return self._position

    def screenPos (self):
        return self._screenPos

    def drawBackground (self, painter, rect):
        painter.fillRect (rect, Qt.gray)

        rect = rect.intersected (self.sceneRect ())

        painter.drawPixmap (rect, self.bgPixmap, rect)
        painter.setPen (QPen (Qt.darkGray, 0, Qt.DotLine))
        painter.drawLines ([QLineF (0, y, rect.width (), y) for y in xrange (0, int (rect.height ()), CHAR_HEIGHT)])
        painter.drawLines ([QLineF (x, 0, x, rect.height ()) for x in xrange (0, int (rect.width ()), CHAR_WIDTH)])

    def mouseMoveEvent (self, e):
        if self.sceneRect ().contains (e.scenePos ()):
            self._position = self.closestPos (e.scenePos ())
            self._screenPos = self.toScreenPos (e.scenePos ())
        super (Screen, self).mouseMoveEvent (e)

    def movePanel (self, panel, target):
        tgtPos = self.toScreenPos (target)
        maxX = self.cols - panel.panelType.minWidth
        maxY = self.rows - panel.panelType.minHeight
        if tgtPos.x () < 0:
            tgtPos.setX (0)
        elif tgtPos.x () > maxX:
            tgtPos.setX (maxX)
        if tgtPos.y () < 0:
            tgtPos.setY (0)
        elif tgtPos.y () > maxY:
            tgtPos.setY (maxY)
        panel.setScreenPos (tgtPos)
        for p in self.items ():
            p.updateOverlapped ()

    def deleteSelected (self):
        for panel in self.items ():
            if panel.isSelected ():
                self.removeItem (panel)

    def keyPressEvent (self, e):
        if e.key () == Qt.Key_Delete:
            self.deleteSelected ()


class ScreenEditor (QGraphicsView):

    def __init__ (self, screen, owner = None):
        super (ScreenEditor, self).__init__ (screen, owner)
        self.setMouseTracking (True)

    def resizeEvent (self, e):
        size = self.sceneRect ().size ().toSize ()
        if e.size ().width () < size.width () or e.size ().height () < size.height ():
            super (ScreenEditor, self).resizeEvent (e)
        else:
            self.fitInView (self.sceneRect (), Qt.KeepAspectRatio)


panelTypes = (
    PanelType ('StableAlt', (7, 1), b'\x85137\x8d', (6, 1)),
    PanelType ('Climb', (7, 1), b'\x04-1.5\x8c', (6, 1)),
    PanelType ('FlightMode', (6, 3), (b'\xd0\xd1\xd1\xd1\xd1\xd2', b'\xd3MODE\xd7', b'\xd4\xd5\xd5\xd5\xd5\xd6')),
    PanelType ('ArmedFlag', (3, 3), (b'\xd8\xd9\xda', b'\xdb\xe0\xdf', b'\xdc\xdd\xde')),
    PanelType ('ConState', (3, 3), (b'\xd8\xd9\xda', b'\xdb\xe1\xdf', b'\xdc\xdd\xde')),
    PanelType ('FlightTime', (7, 1), b'\xb312:34', (6, 1)),
    PanelType ('Roll', (7, 1), b'\xb2-15\xb0', (5, 1)),
    PanelType ('Pitch', (7, 1), b'\xb110\xb0', (5, 1)),
    PanelType ('GPS', (6, 1), b'\x10\x11\x0212', (5, 1)),
    PanelType ('Lat', (10, 1), b'\x8356.833233'),
    PanelType ('Lon', (10, 1), b'\x8460.583333'),
)

panelTypesByName = {pt.name: pt for pt in panelTypes}


class Win (QWidget):

    def __init__ (self):
        super (Win, self).__init__ ()
        self.screen = Screen (30, 16, Font (open ('main_font.mcm', 'rb')), QImage ('background.jpg'), owner = self)
        self.view = ScreenEditor (self.screen, self)

        [Panel (self.screen, self.screen.fixedFont, _) for _ in panelTypes]

        self.l_main = QVBoxLayout (self)
        self.l_main.addWidget (self.view)


app = QApplication (sys.argv)

#s = Screen (30, 16, Font (open ('main_font.mcm', 'rb')), QImage ('background.jpg'))
w = Win ()
w.resize (1100, 800)

w.show ()

sys.exit (app.exec_ ())

