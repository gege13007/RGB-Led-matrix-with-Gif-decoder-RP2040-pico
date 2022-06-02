/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "matrix.h"
#include "gif.h"

//========== Prog Principal ========
int main() {

 Init_GPIO();
 
 multicore_launch_core1(core2);
 
 while (1) {
/*  uint ang = 0;
  uint8_t freq = 9;
 
   while (1) {
   ang += 2; 
   
   for (uint8_t x=0; x<64; x++) {
     for (uint8_t y=0; y<32; y++) {
       float y = 15+15*sin((ang+freq*x) / 57.29578);
       uint8_t yi= y;
       Pset(x, yi, (y-16)*7, 0, 255-9*y);
     }
   }
   sleep_ms(2);
   
   for (uint8_t x=0; x<64; x++) {
     for (uint8_t y=0; y<32; y++) {
       float y = 15+15*sin((ang+(freq-2)*x) / 57.29578);
       uint8_t yi= y;
       Pset(x, yi, 0, 0, 0);
     }
   }
   if (ang>356) break;
 }  */

   process_gif();
   
 }
 
}
