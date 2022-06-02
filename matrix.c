/**
 * Copyright (c) 2022 G.Samblancat Ltd.
 */
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "matrix.h"
#include "font88.h"

uint8_t gamma8[] = {
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  1,  1,  1,  1,
    1,  1,  1,  1,  1,  1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,
    2,  3,  3,  3,  3,  3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  5,
    5,  6,  6,  6,  6,  7,  7,  7,  7,  8,  8,  8,  9,  9,  9, 10,
   10, 10, 11, 11, 11, 12, 12, 13, 13, 13, 14, 14, 15, 15, 16, 16,
   17, 17, 18, 18, 19, 19, 20, 20, 21, 21, 22, 22, 23, 24, 24, 25,
   25, 26, 27, 27, 28, 29, 29, 30, 31, 32, 32, 33, 34, 35, 35, 36,
   37, 38, 39, 39, 40, 41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 50,
   51, 52, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 66, 67, 68,
   69, 70, 72, 73, 74, 75, 77, 78, 79, 81, 82, 83, 85, 86, 87, 89,
   90, 92, 93, 95, 96, 98, 99,101,102,104,105,107,109,110,112,114,
  115,117,119,120,122,124,126,127,129,131,133,135,137,138,140,142,
  144,146,148,150,152,154,156,158,160,162,164,167,169,171,173,175,
  177,180,182,184,186,189,191,193,196,198,200,203,205,208,210,213,
  215,218,220,223,225,228,231,233,236,239,241,244,247,249,252,255 };
  
//==== Proc de rafraichissement ecran =====
void setRow(uint8_t row) {
  gpio_put(G_A, ((row & 1)==0)?0:1);
  gpio_put(G_B, ((row & 2)==0)?0:1);
  gpio_put(G_C, ((row & 4)==0)?0:1);
  gpio_put(G_D, ((row & 8)==0)?0:1);
}

void Pset(uint16_t x,uint16_t y,uint8_t RR,uint8_t GG,uint8_t BB) {
  xRED[x][y] = gamma8[RR];
  xGREEN[x][y] = gamma8[GG];
  xBLUE[x][y] = gamma8[BB];
}

void Home(void) {
  CurX=0;
  CurY=0;
}

void SetXY(uint16_t x, uint16_t y) {
  CurX = x;
  CurY = y;
}

void PrintChar(uint8_t ch) {
  for (uint8_t x=0; x<8; x++) {
    for (uint8_t y=0; y<8; y++) {
      uint8_t lum= font8[ch][y][x];
      if (lum!=0) {
        xRED[x+CurX][CurY+y] = 30;
        xGREEN[x+CurX][CurY+y] = 240;
        xBLUE[x+CurX][CurY+y] = 20;
      } else {
        xRED[x+CurX][CurY+y] = 0;
        xGREEN[x+CurX][CurY+y] = 0;
        xBLUE[x+CurX][CurY+y] = 0;
      }
    }
  }
  CurX += 8;
  if (CurX >= nMatrix_X * nLedsMatrix_X) {
     CurY += 9;
     CurX = 0;
  }
}

void PrintHex(uint8_t nl) {
  uint8_t msb = nl >> 4;
  if (msb > 9) msb+=7;
  PrintChar(msb);
  
  uint8_t lsb = nl & 15;
  if (lsb > 9) lsb+=7;
  PrintChar(lsb);
}

void PrintHex2(uint16_t nn) {
  uint8_t xx = (nn / 256);
  PrintHex(xx);
  xx = nn & 255;
  PrintHex(xx);
}


//------- BALAYAGE PANNEAU RGB - TACHE DE FOND - MULTICORE 2 -------
void core2() {
uint8_t frame = 0;
    
while (1) {
  if (!gpio_get(BOOT_BUTTON)) reset_usb_boot(0,0);
  
 for (uint8_t rows=0; rows<16; rows++) {
   gpio_put(G_OE, 0);
   gpio_put(G_LAT, 0);
    
   for (uint8_t col=0; col< nMatrix_X * 32; col++) {
     if (xRED[col][rows] > frame) gpio_put(G_R1,1); else gpio_put(G_R1,0);
     if (xGREEN[col][rows] > frame) gpio_put(G_G1,1); else gpio_put(G_G1,0);
     if (xBLUE[col][rows] > frame) gpio_put(G_B1,1); else gpio_put(G_B1,0);
     
     if (xRED[col][rows+16] > frame) gpio_put(G_R2,1); else gpio_put(G_R2,0);
     if (xGREEN[col][rows+16] > frame) gpio_put(G_G2,1); else gpio_put(G_G2,0);
     if (xBLUE[col][rows+16] > frame) gpio_put(G_B2,1); else gpio_put(G_B2,0);
     
	   gpio_put(G_CLK, 1);     // Tempo sur CLK
     gpio_put(G_CLK, 1);
     gpio_put(G_CLK, 1);
     gpio_put(G_CLK, 0);
   }
   
   gpio_put(G_OE, 1);

   gpio_put(G_LAT, 1);
   setRow(rows);
   gpio_put(G_LAT, 1);
   gpio_put(G_LAT, 0);
   gpio_put(G_OE, 0);
 }
 frame += 16;
}
}


void Init_GPIO() {
 stdio_init_all();
  
 gpio_init(G_OE);
 gpio_init(G_CLK);
 gpio_init(G_LAT);
 gpio_init(G_R1);
 gpio_init(G_G1);
 gpio_init(G_B1);
 gpio_init(G_R2);
 gpio_init(G_G2);
 gpio_init(G_B2);
 gpio_init(G_A);
 gpio_init(G_B);
 gpio_init(G_C);
 gpio_init(G_D);

 gpio_init(BOOT_BUTTON);
 gpio_pull_up(BOOT_BUTTON);

 gpio_set_dir(G_OE, GPIO_OUT);
 gpio_set_dir(G_CLK, GPIO_OUT);
 gpio_set_dir(G_LAT, GPIO_OUT);
 gpio_set_dir(G_R1, GPIO_OUT);
 gpio_set_dir(G_G1, GPIO_OUT);
 gpio_set_dir(G_B1, GPIO_OUT);
 gpio_set_dir(G_R2, GPIO_OUT);
 gpio_set_dir(G_G2, GPIO_OUT);
 gpio_set_dir(G_B2, GPIO_OUT);
 gpio_set_dir(G_A, GPIO_OUT);
 gpio_set_dir(G_B, GPIO_OUT);
 gpio_set_dir(G_C, GPIO_OUT);
 gpio_set_dir(G_D, GPIO_OUT);

 gpio_set_drive_strength(G_OE, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_CLK, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_LAT, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_A, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_B, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_C, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_D, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_R1, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_G1, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_B1, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_R2, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_G2, GPIO_DRIVE_STRENGTH_12MA);
 gpio_set_drive_strength(G_B2, GPIO_DRIVE_STRENGTH_12MA);
   
 gpio_set_dir(BOOT_BUTTON, GPIO_IN);
}
