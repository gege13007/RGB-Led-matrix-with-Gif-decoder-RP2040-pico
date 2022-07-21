# Controlling RGB LED display with a PICO Raspberry Pi RP2040

## Overview
This is a simple implementation of an RGB Led panel (matrix.c) with a RP2040 Pico Raspberry (price 6 €).
This library is designed to control up to 8 32x32 RGB LED panels connected with a single chain to the Raspberry Pico RP2040.

Even overclocked at 200MHz, the PWM colors palette has to be limited to 4 Bits per channel with 8 panels.
It's even possible to improve the number of colors with less panel. So this implementation is cheap but limited to simple pictures.
Don't try to display complex pictures with subtle color !

This library also include a build in GIFAnim decoder (gif.c).

![open pizza flashing](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/open-pizza.jpg)
Example of flashing panel 2 x 4 x 32 x 32 leds panels

![green arrows](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/green-arrow-6_105644.jpg)
Example of moving arrow screen panel 6 x 32x32 leds panels

![6 leds panels and a pico](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/panels-6-leds-rear_11014.jpg)

![the Pico raspberry on led panels](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/rgb-leds-pico-rear_105537.jpg)
Six matrix chained, with a Pico and a power supply

## Types of display

## Connection
The GPIO mapping and all connections are described in matrix.h. The control of the chained dot-matrix is made by matric.c. This task needs for speed, and is designed to run in the second of the two cores of our RP2040.

| HUB75 Pin | Pico Pin | Function | 
| :---      |  :---:  | :---: |
| OE | 0 | Led Output enable | 
| LAT | 1 | Latche shifted datas|
| CLK | 2 | Clock of shifted datas|
| A | 4 | Line Selection 1 |
| B | 5 | Line Selection 2 |
| C | 6 | Line Selection 4 |
| D | 7 | Line Selection 8 |
| R2 | 8 | Red interlaced 16/32 |
| G2 | 9 | Greeen interlaced 16/32|
| B2 | 10 | Blue interlaced 16/32|
| R1 | 11 | Red interlaced 1/16|
| G1 | 12 | Green interlaced 1/16|
| B1 | 13 | Blue interlaced 1/16|

| Other GPIO | Pico Pin | Function | 
| :---    |  :---:  | :---: |
| BOOT_BUTTON | 22  | Reset BOOTSEL |
| LED_PIN | 25 | DEFAULT_LED_PIN |
| BUTTON1 | 16 | Optional Button 1 |
| BUTTON2 | 17 | Optional Button 2 |

## Limitations
For now, the PICO was tested only with 32x32 leds matrix, and up to 8 matrix (256 x 32 pixels). This 8 matrix can be assembled in 2 lines of 4 matrix (reverse Z-stripe 128x64 pixels) or in a single line (256x32 pixels). In both case I think it's a MAXIMUM for a small Pico (even overclocked at 200MHz). And in this configuration we have only 256/32 = 8 levels only for each colors ( 512 colors max ).

## Utilities

