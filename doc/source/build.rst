*****************
MultiOSD Building
*****************


Debian/Ubuntu
=============


1. Prepare your system:

::

   # apt-get install gcc-avr avr-libc

2. Clone the repo:

::

   $ git clone https://github.com/UncleRus/MultiOSD.git

3. Make it:

::

   $ cd MultiOSD
   $ make

4. Select suitable version of firmware...:

::

   $ ls build/

5. ...and flash it like this:

::

   $ avrdude -pm328p -carduino -P/dev/ttyUSB0 -Uflash:w:build/MultiOSD_uavtalk_adcbattery_adcrssi.hex:a
   

   