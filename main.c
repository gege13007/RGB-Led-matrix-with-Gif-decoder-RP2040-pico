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
#include "Arrow256_32_12gif.h"
#include "GArrows_256_32bgif.h"
#include "open-siz-128-64gif.h"

/*void gpio_callback(uint gpio, uint32_t events) {
  if (gpio_get(LED_PIN)!=0)
    gpio_put(LED_PIN, 0);  //TEST ONLY
  else
    gpio_put(LED_PIN, 1);

  gpio_acknowledge_irq(gpio, events);
} */


//Efface ecran
void clear_screen(void) {
 for (uint16_t x=0; x < nb_pixel_X; x++) {
   for (uint16_t y=0; y < nb_pixel_Y; y++) {
     PsetnoG(x, y, 0, 0, 0);
   }
  }
}
 

void test_screen(void) {
uint16_t totx = zStripes_X * nLedsMatrix_X;

  for (uint8_t dt=0; dt<16; dt+=4) {
   //tOP H
   for (uint16_t x=dt; x < totx-dt; x++) {
     PsetnoG(x, dt, 248, 2, 2);
     sleep_ms(1);
     if (x==nb_pixel_X-1) break;
   }
   //Right V
   for (uint8_t y=dt; y < nb_pixel_Y-dt; y++) {
     PsetnoG(totx-1-dt, y, 2, 248, 2);
     sleep_ms(2);
   }
   //bottOm H
   for (uint16_t x=totx-1-dt; ; x--) {
     PsetnoG(x, nb_pixel_Y-1-dt, 80, 80, 80);
     sleep_ms(1);
     if (x==dt) break;
   }
   //Left V
   for (uint8_t y=nb_pixel_Y-1-dt; ; y--) {
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
 
 //test & Efface ecran
 test_screen();
 sleep_ms(3000);
 clear_screen();
 
  while (1) {
       
 // process_gif(chat);
 process_gif(GArrows_256_32bgif);
  process_gif(GArrows_256_32bgif);
 process_gif(GArrows_256_32bgif);
 process_gif(GArrows_256_32bgif);
  process_gif(GArrows_256_32bgif);
  process_gif(GArrows_256_32bgif);
 process_gif(GArrows_256_32bgif);
 process_gif(GArrows_256_32bgif);
 Fade_out();
   
 process_gif(Arrow256_32_12gif);
 process_gif(Arrow256_32_12gif);
  process_gif(Arrow256_32_12gif);
 process_gif(Arrow256_32_12gif);
 process_gif(Arrow256_32_12gif);
 process_gif(Arrow256_32_12gif);
  process_gif(Arrow256_32_12gif);
 process_gif(Arrow256_32_12gif);
 Fade_out();

 process_gif(open_siz_128_64gif);
   
//  gpio_set_irq_enabled_with_callback(TEST_PIN, GPIO_IRQ_EDGE_FALL, true, &gpio_callback);
 

 while (1) {
   if (gpio_get(BUTTON2)==0) {
     process_gif(Arrow256_32_12gif);
   }

   if (gpio_get(BUTTON1)==0) {
     process_gif(GArrows_256_32bgif);
   }
   
   if ( (gpio_get(BUTTON1)!=0) && (gpio_get(BUTTON2)!=0) ) break;
 }
 
 Fade_out();
 
  while ( (gpio_get(BUTTON1)!=0) && (gpio_get(BUTTON2)!=0) ) {
    clear_screen();    
  }
 
}

}
