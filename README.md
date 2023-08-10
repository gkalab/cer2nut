# cer2nut

Cer2nut adapts the protocol of a Certabo chess e-board to the Chessnut protocol used in the Chessnut Air and Chessnut Pro e-boards.

The adapter allows software written to support the Chessnut e-boards to be used with a Certabo e-board.
Not all software written for the Chessnut protocol may work with this adapter and there is no guarantee that this adapter will work in the future.

Hardware requirements
---------------------

This software is only compatible with development boards having a ESP32-S3-WROOM-1 chip and a USB OTG port on board.

Software requirements
---------------------

ESP-IDF >= 5.1 https://idf.espressif.com/

Connection
----------

**Use at Own Risk.** The Software is licensed for free under the GNU general public license version 3 and “as is” and 
may not operate properly, be in final form or fully functional, and may have errors, bugs, design flaws, and other defects.
Use of the software is at your own risk, the author is not responsible for any damage to your equipment when using this software!

After flashing the firmware, connect the USB OTG port to the Certabo E-Board and the second USB port to the power supply.

![Alt text](connection_diagram.png?raw=true "Connection diagram")