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

![6 leds panels and a pico](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/panels-6-leds-rear_11014.jpg)

![the Pico raspberry on led panels](https://github.com/gege13007/RGB-Led-matrix-with-Gif-decoder-RP2040-pico/blob/main/rgb-leds-pico-rear_105537.jpg)

## Types of display

## Connection

## Limitations

## Utilities

