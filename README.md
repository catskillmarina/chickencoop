Chicken coop controller code. Mostly BSD Licensed

Hardware is a Raspberry Pi for the computer running
NetBSD 7.0. Shell is Bash with bits of expect. The expect
code will be replaced by C code running termio stuff - far more
elegant. The switching is done with a USB-Serial Relay the 4xRelay-USB
switches that take simple text commands over serial over USB.
The Lights will be controlled by an HVAC relay controlled by one of
the USB relays. The next switch - switch #2 will be controlling the
current to an old car power window motor to open and close the door.

The CODE that is mine is BSD licensed
