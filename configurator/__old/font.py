# -*- coding: utf-8 -*-

from PySide.QtCore import *
from PySide.QtGui import *
import sys


py3 = sys.version_info [0] >= 3
if py3:
    xrange = range


class Char (QImage):

    colors = (
        qRgb (0, 0, 0),
        qRgba (128, 128, 128, 0),
        qRgb (255, 255, 255),
        qRgba (128, 128, 128, 0),
    )

    def __init__ (self, data):
        super (Char, self).__init__ (QSize (12, 18), QImage.Format.Format_Indexed8)
        for i, c in enumerate (self.colors):
            self.setColor (i, c)

        for r in xrange (18):
            for c in xrange (3):
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
            dest = QImage (QSize (maxlen * 12, len (textbuf) * 18), QImage.Format.Format_ARGB32_Premultiplied)
            dest.fill (qRgba (0, 0, 0, 0))

        painter = QPainter (dest)
        pos = QPoint (0, 0)
        for line in textbuf:
            pos.setX (0)
            for c in bytearray (line):
                painter.drawImage (pos, self.chars [c])
                pos.setX (pos.x () + 12)
            pos.setY (pos.y () + 18)
        painter.end ()

        return dest


class Panel (QLabel):

    def __init__ (self, image, owner = None):
        super (Panel, self).__init__ (owner)
        self.setPixmap (QPixmap.fromImage (image))


class Screen (QWidget):

    def __init__ (self, *args, **kwargs):
        super (Screen, self).__init__ (*args, **kwargs)
        self._bg = None

    def getBgImage (self):
        return self._bg

    def setBgImage (self, value):
        if value:
            self._bg = QImage (value)

    bgImage = Property (QImage, getBgImage, setBgImage)

    def paintEvent (self, event):
        painter = QPainter (self)
        rect = event.rect ()
        if self._bg:
            painter.drawImage (rect, self._bg, rect)
        else:
            painter.fillRect (rect, Qt.blue)



def read_mcm (f):
    if f.readline ().strip () != 'MAX7456':
        raise IOError ('Unknown file format')
    return [int (f.readline ().strip (), 2) for _ in xrange (16384)]


#def draw_screen (painter, panels, chars):
#    # трансформируем размеры пайнтера
#    # выводим картинки

app = QApplication ([])

#fnt = read_mcm (open ('main_font.mcm', 'rb'))
#chars = [Char (fnt [i * 64:]) for i in xrange (256)]

font = Font (open ('main_font.mcm', 'rb'))
font.write ((b'Hello world! 12343 01 \x09\x08\x07\xfd', 'Second line'))

#lbl = QLabel ()
#lbl.setPixmap (QPixmap.fromImage (chars [10]))
#lbl.setPixmap (QPixmap.fromImage (font.write ((b'Hello world! 12343 01 \x09\x08\x07\xfd', 'Second line'))))
#lbl.show ()
s = Screen ()
s.bgImage = QImage ('/home/rus/work/home/min_raw_settings/src/11.0.jpg')
s.show ()

sys.exit (app.exec_ ())
