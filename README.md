# cer2nut

Cer2nut adapts the protocol of a Certabo chess e-board to the Chessnut protocol used in the Chessnut Air and Chessnut Pro e-boards.

The adapter allows software written to support the Chessnut e-boards to be used with a Certabo e-board or a Tabutronic Sentio e-board.
Not all software written for the Chessnut protocol may work with this adapter and there is no guarantee that this adapter will work in the future.

## Hardware requirements

This software is only compatible with development boards having a ESP32-S3-WROOM-1 chip and a USB OTG port on board.

## Software requirements


ESP-IDF == 5.2.1 https://github.com/espressif/esp-idf/releases/tag/v5.2.1

## Connection

**Use at Own Risk.** The Software is licensed for free under the GNU general public license version 3 and “as is” and 
may not operate properly, be in final form or fully functional, and may have errors, bugs, design flaws, and other defects.
Use of the software is at your own risk, the author is not responsible for any damage to your equipment when using this software!

After flashing the firmware, connect the USB OTG port to the Certabo E-Board and the second USB port to the power supply.

![Alt text](connection_diagram.png?raw=true "Connection diagram")

## Usage

### Certabo e-boards

Place the chess pieces on the corresponding squares of the starting position. Place any additional queens on d3 and d6. On the first connection
the pieces are registered. This calibration takes a few seconds and must be repeated if there is a power interruption.

### Tabutronic Sentio e-boards

Place the chess pieces on the corresponding squares of the starting position. Do not place additional queens on the board.
Since the hardware of Sentio e-boards does not support piece recognition, there is one limitation:

* Chess games can only be started from the normal chess starting position. At the end of a game, place all the pieces back to the starting position.

#### Underpromotion

By default, a pawn is promoted to a queen. To promote to a knight, bishop or rook, use the following sequence:

1. pick up the pawn to promote
2. pick up the king of the same color
3. put down the king --> the promotion piece becomes a knight
4. put down the promotion piece on the promotion square
    
- Follow steps 2+3 twice to convert the promotion piece into a bishop.
- Follow steps 2+3 three times to convert the promotion piece into a rook.
- Follow steps 2+3 four times to convert the promotion piece back into a queen.

The change of the promotion piece is only valid for the next promotion; any other move resets the promotion piece back to a queen.

### Tabutronic Spectrum e-boards

To change the brightness of the LEDs, place all the pieces in their starting positions and then place the white king on A3, B3, ..., or H3. 
Placing the white king on A3 selects the lowest brightness, while placing it on H3 selects the highest brightness.

### Switching between e-boards

When switching between e-boards, always turn off the module before changing boards.

## Example videos

Playing on a Certabo e-board with the official Chessnut app for Android:

[![Certabo to Chessnut adapter using an ESP32-S3 development board](certabo_video.jpg?raw=true)](https://youtu.be/TdJrJ_2FbKA "Certabo to Chessnut adapter using an ESP32-S3 development board")

Playing on a Tabutronic Sentio e-board with the official Chessnut app for iOS:

[![Tabutronic Sentio disguised as Chessnut Air](sentio_video.jpg?raw=true)](https://youtu.be/Mfb8jt4EoKU "Tabutronic Sentio disguised as Chessnut Air")
