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
    0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,  1,
    1,  1,  1,  1,  2,  2,  2,  2,  2,  2,  2,  2,  2,  3,  3,  3,
    3,  3,  3,  4,  4,  4,  4,  4,  5,  5,  6,  7,  8,  9,  9, 10,
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
  if (x < nb_pixel_X) {
    if (y < nb_pixel_Y) {
      xRED[x][y] = gamma8[RR];
      xGREEN[x][y] = gamma8[GG];
      xBLUE[x][y] = gamma8[BB];
    }
  }
}

void PsetnoG(uint16_t x,uint16_t y,uint8_t RR,uint8_t GG,uint8_t BB) {
  if (x < nb_pixel_X) {
    if (y < nb_pixel_Y) {
      xRED[x][y] = RR;
      xGREEN[x][y] = GG;
      xBLUE[x][y] = BB;
    }
  }
}

void Home(void) {
  CurX=0;
  CurY=0;
}

void SetXY(uint16_t x, uint16_t y) {
  CurX = x;
  CurY = y;
}

//Efface ecran
void Fade_out(void) {
 for (uint16_t l=0; l<255; l++) {
   for (uint16_t x=0; x < nb_pixel_X; x++) {
     for (uint8_t y=0; y < nb_pixel_Y; y++) {
       if (xRED[x][y]!=0) xRED[x][y]=xRED[x][y]-1;
       if (xGREEN[x][y]!=0) xGREEN[x][y]=xGREEN[x][y]-1;
       if (xBLUE[x][y]!=0) xBLUE[x][y]=xBLUE[x][y]-1;
     }
   }
   sleep_us(500);
 }
}


//------- BALAYAGE PANNEAU RGB - TACHE DE FOND - MULTICORE 2 -------
void core2() {
uint16_t frame = 0;
uint16_t xpan, col, col0, col2, rows, rows2, rows216, rows16;
uint16_t Ymatrix_1, Ymatrix_2_1;

 while (1) {   
   if (!gpio_get(BOOT_BUTTON)) reset_usb_boot(0,0);
       
   //TEST ONLY
/*   if (gpio_get(TEST_PIN)!=0)
     gpio_put(TEST_PIN, 0);  //TEST ONLY
   else
     gpio_put(TEST_PIN, 1);
*/   
   for (rows=0; rows<16; rows++) {
     //pré-calcul
     rows2 = rows + nLedsMatrix_Y;
     rows216 = rows2 + 16;
     rows16 = rows + 16;
     
     Ymatrix_2_1 = 15;   //(nLedsMatrix_Y >> 2) - 1;
     Ymatrix_1 = nLedsMatrix_Y - 1;
     
     gpio_put(G_OE, 0);
     gpio_put(G_LAT, 0);
     
     for (xpan=0; xpan < nMatrix_X; xpan++) {
       if (xpan > zStripes_X-1) {   // xpan = 2, 3 (matrices dans le bon sens)
     
         for ( col=nLedsMatrix_X*(xpan-zStripes_X); col < nLedsMatrix_X*(1+xpan-zStripes_X); col++) {
           
           if (xRED[col][rows2] > frame) gpio_put(G_R1,1); else gpio_put(G_R1,0);
           if (xGREEN[col][rows2] > frame) gpio_put(G_G1,1); else gpio_put(G_G1,0);
           if (xBLUE[col][rows2] > frame) gpio_put(G_B1,1); else gpio_put(G_B1,0);
           
           if (xRED[col][rows216] > frame) gpio_put(G_R2,1); else gpio_put(G_R2,0);
           if (xGREEN[col][rows216] > frame) gpio_put(G_G2,1); else gpio_put(G_G2,0);
           if (xBLUE[col][rows216] > frame) gpio_put(G_B2,1); else gpio_put(G_B2,0);
	         
           gpio_put(G_CLK, 1);     // Tempo sur CLK
           gpio_put(G_CLK, 1);
           gpio_put(G_CLK, 1);
           gpio_put(G_CLK, 0);
         }
         
       } else {    // xpan = 0, 1 (matrices inversées tête-bèche)
     
         for (col = nLedsMatrix_X*(zStripes_X-xpan); col > nLedsMatrix_X*((zStripes_X-1-xpan)); col--) {
        
           if (xRED[col-1][Ymatrix_1 - rows] > frame) gpio_put(G_R1,1); else gpio_put(G_R1,0);
           if (xGREEN[col-1][Ymatrix_1 - rows] > frame) gpio_put(G_G1,1); else gpio_put(G_G1,0);
           if (xBLUE[col-1][Ymatrix_1 - rows] > frame) gpio_put(G_B1,1); else gpio_put(G_B1,0);
           
           if (xRED[col-1][Ymatrix_2_1 - rows] > frame) gpio_put(G_R2,1); else gpio_put(G_R2,0);
           if (xGREEN[col-1][Ymatrix_2_1 - rows] > frame) gpio_put(G_G2,1); else gpio_put(G_G2,0);
           if (xBLUE[col-1][Ymatrix_2_1 - rows] > frame) gpio_put(G_B2,1); else gpio_put(G_B2,0);
           
	         gpio_put(G_CLK, 1);     // Tempo sur CLK
           gpio_put(G_CLK, 1);
           gpio_put(G_CLK, 1);
           gpio_put(G_CLK, 0);
         }       
       }
       
 /*      for (col0=0; col0 < nLedsMatrix_X; col0++) {
         if (xpan >= zStripes_X) {
           col2 = col0 + (nLedsMatrix_X * (xpan-zStripes_X));
           if (xRED[col2][rows2] > frame) gpio_put(G_R1,1); else gpio_put(G_R1,0);
           if (xGREEN[col2][rows2] > frame) gpio_put(G_G1,1); else gpio_put(G_G1,0);
           if (xBLUE[col2][rows2] > frame) gpio_put(G_B1,1); else gpio_put(G_B1,0);
           if (xRED[col2][rows2+16] > frame) gpio_put(G_R2,1); else gpio_put(G_R2,0);
           if (xGREEN[col2][rows2+16] > frame) gpio_put(G_G2,1); else gpio_put(G_G2,0);
           if (xBLUE[col2][rows2+16] > frame) gpio_put(G_B2,1); else gpio_put(G_B2,0);
         }
         else {
           col = col0 + (nLedsMatrix_X * xpan);
           if (xRED[col][rows] > frame) gpio_put(G_R1,1); else gpio_put(G_R1,0);
           if (xGREEN[col][rows] > frame) gpio_put(G_G1,1); else gpio_put(G_G1,0);
           if (xBLUE[col][rows] > frame) gpio_put(G_B1,1); else gpio_put(G_B1,0);
           if (xRED[col][rows+16] > frame) gpio_put(G_R2,1); else gpio_put(G_R2,0);
           if (xGREEN[col][rows+16] > frame) gpio_put(G_G2,1); else gpio_put(G_G2,0);
           if (xBLUE[col][rows+16] > frame) gpio_put(G_B2,1); else gpio_put(G_B2,0);
         }
       }  */
      
     }
     
     gpio_put(G_OE, 1);
     gpio_put(G_LAT, 1);
     setRow(rows);
     gpio_put(G_LAT, 0);
     gpio_put(G_OE, 0);
   }
   
   frame += 32;
   if (frame>255) frame=0;
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

 gpio_init(BUTTON1);
 gpio_init(BUTTON2);
 gpio_init(LED_PIN);

 gpio_init(BOOT_BUTTON);
 
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
 
 gpio_set_dir(BUTTON1, GPIO_IN);
 gpio_set_dir(BUTTON2, GPIO_IN);
 
 gpio_set_dir(LED_PIN, GPIO_OUT);
 
 gpio_set_dir(BOOT_BUTTON, GPIO_IN);
 
 gpio_pull_up(BUTTON1);
 gpio_pull_up(BUTTON2);
 gpio_pull_up(BOOT_BUTTON);
  
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
