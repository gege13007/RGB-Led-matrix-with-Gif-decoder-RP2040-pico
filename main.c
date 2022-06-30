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

//GIF Image en tableau
#include "catsnouille_96_64gif.h"        // cat[.]
#include "arrows3-right-94-64gif.h"      // dino[.]

/*void gpio_callback(uint gpio, uint32_t events) {
  if (gpio_get(LED_PIN)!=0)
    gpio_put(LED_PIN, 0);  //TEST ONLY
  else
    gpio_put(LED_PIN, 1);

  gpio_acknowledge_irq(gpio, events);
} */


//Efface ecran
void clear_screen(void) {
 for (uint8_t x=0; x < max_pixel_X; x++) {
     for (uint8_t y=0; y < max_pixel_Y; y++) {
       PsetnoG(x, y, 0, 0, 0);
     }
  }
}
 

void test_screen(void) {
  for (uint8_t dt=0; dt<30; dt+=4) {
   //tOP H
   for (uint8_t x=dt; x < max_pixel_X-dt; x++) {
     PsetnoG(x, dt, 248, 2, 2);
     sleep_ms(1);
     if (x==max_pixel_X-1) break;
   }
   //Right V
   for (uint8_t y=dt; y < max_pixel_Y-dt; y++) {
     PsetnoG(max_pixel_X-1-dt, y, 2, 248, 2);
     sleep_ms(2);
   }
   //bottOm H
   for (uint8_t x=max_pixel_X-1-dt; ; x--) {
     PsetnoG(x, max_pixel_Y-1-dt, 80, 80, 80);
     sleep_ms(1);
     if (x==dt) break;
   }
   //Left V
   for (uint8_t y=max_pixel_Y-1-dt; ; y--) {
     PsetnoG(dt, y, 2, 2, 250);
     sleep_ms(2);
     if (y==dt+1) break;
   }
  }
  sleep_ms(900);
}

//========== Prog Principal ========
int main() {

 Init_GPIO();
 
 set_sys_clock_khz(200000, true);
 
 multicore_launch_core1(core2);
 
 //Efface ecran
 clear_screen();
  
 test_screen();

//  gpio_set_irq_enabled_with_callback(TEST_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
 
 while (1) {
   if (gpio_get(BUTTON2)==0) {
      process_gif(pixgif2);
      //Efface ecran
      Fade_out();
   }

   if (gpio_get(BUTTON1)==0) {
     process_gif(arrows3);
     Fade_out();
   }
 }
}
