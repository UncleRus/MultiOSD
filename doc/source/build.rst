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


MultiOSD installation
=====================

You will need the USB-UART module like FT-232 or pl2303. Connect it to MinimOSD
UART.

.. todo:: Diagram


1. Install python, pip and libraries:

::

   # apt-get install python python-pip
   ...
   $ pip install -r tools/requirements.txt

2. Select suitable version of firmware:

::

   $ ls build/
   MultiOSD_mavlink_adcrssi.hex  MultiOSD_mavlink.hex  MultiOSD_uavtalk_adcbattery_adcrssi.hex  MultiOSD_uavtalk_adcbattery.hex  MultiOSD_uavtalk_adcrssi.hex  MultiOSD_uavtalk.hex

3. Flash it like this:

::

   $ python tools/flasher.py -p /dev/ttyUSB0 build/MultiOSD_uavtalk_adcbattery_adcrssi.hex

4. Upload MAX7456 font:

::
   
   $ python tools/font_uploader.py -p /dev/ttyUSB0 res/font.mcm
