Console
=======

MultiOSD offers low-level configuration console. To use it you will need
terminal software and COM-port (it can be any USB-UART connector, like FTDI
or Prolific pl2303).

1. Run your terminal software.
2. Connect MultiOSD to COM-port.
3. Open port with baudrate **57600**.
4. Press reset button on MultiOSD.
5. Wait for message ``READY``.
6. Type ``config`` and press ``Enter``. Entered characters will not be displayed.
   If you were mistaken you'll see ``BOOT`` message. Just press reset "Reset" and
   try again.
7. After that you'll see command promt

::
   
   MultiOSD v.0010
   osd#
   
8. Type ``help`` to see commands.

