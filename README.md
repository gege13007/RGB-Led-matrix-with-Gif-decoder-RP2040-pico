# Controlling RGB LED display with a PICO Raspberry Pi RP2040

## Overview
This is a simple implementation of an RGB Led panel (matrix.c) with a RP2040 Pico Raspberry (price 6 €).
This library is designed to control up to 8 32x32 RGB LED panels connected with a single chain to the Raspberry Pico RP2040.

Even overclocked at 200MHz, the PWM colors palette has to be limited to 4 Bits per channel with 8 panels.
It's even possible to improve the number of colors with less panel. So this implementation is cheap but limited to simple pictures.
Don't try to display complex pictures with subtle color !

This library also include a build in GIFAnim decoder (gif.c).

Here's my prototype with 6 x 32x32 leds panels

![green arrows](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/green-arrow-6_105644.jpg)
example of arrow scrrren panel

![6 leds panels and a pico](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/panels-6-leds-rear_11014.jpg)
six matrix are chained, a Pico, and a power supply

![the Pico raspberry on led panels](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/rgb-leds-pico-rear_105537.jpg)

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

| BOOT_BUTTON | 22  | Reset BOOTSEL |
| LED_PIN | 25 | DEFAULT_LED_PIN |

| BUTTON1 | 16 | Optional Button 1 |
| BUTTON2 | 17 | Optional Button 2 |

## Limitations

## Utilities

