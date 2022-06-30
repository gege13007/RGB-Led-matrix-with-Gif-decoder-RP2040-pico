/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 */
#define G_OE   0
#define G_LAT  1
#define G_CLK  2

#define G_A    4
#define G_B    5
#define G_C    6
#define G_D    7

#define G_R2   8
#define G_G2   9
#define G_B2   10
#define G_R1   11
#define G_G1   12
#define G_B1   13

#define BOOT_BUTTON 22   // Reset par BOOTSEL

#define LED_PIN   25     // PICO_DEFAULT_LED_PIN

#define BUTTON1  16      // Bouton CTL 1
#define BUTTON2  17      // Bouton CTL 2

// - - - - Description PHYSIQUE de L'écran
// !!! TOUTES MATRICES SONT SUR UNE SEULE LIGNE !!!!
static uint8_t nMatrix_Y = 1;
static uint8_t nMatrix_X = 6;

//Si >0 alors mapping des panneaux en ZStripe (zigzag) de x large
static uint8_t zStripes_X = 3;

// Nombre de LEDS par Matrices
static uint8_t nLedsMatrix_Y = 32;
static uint8_t nLedsMatrix_X = 32;

// - - - - Description LOGIQUE de L'écran - - - - - - 
// Dimensions du Buffer logique BITMAP en PIXELS X & Y
#define max_pixel_X        96     // donne 4 x 40960 oc. = 164 ko / 264ko de RAM
#define max_pixel_Y        64     // LIMITE par la RAM du PICO

static uint8_t xRED[max_pixel_X][max_pixel_Y] = {0};
static uint8_t xGREEN[max_pixel_X][max_pixel_Y] = {0};
static uint8_t xBLUE[max_pixel_X][max_pixel_Y] = {0};

//Curseur next PrintChar
static uint8_t CurX=0;
static uint8_t CurY=0; 
 
//============== Declarations ===========
void Init_GPIO();
void setRow(uint8_t row);
void PrintChar(uint8_t );
void PrintHex(uint8_t );
void PrintHex2(uint16_t nn);
void Pset(uint16_t x,uint16_t y,uint8_t RR,uint8_t GG,uint8_t BB);
void PsetnoG(uint16_t x,uint16_t y,uint8_t RR,uint8_t GG,uint8_t BB);
void core2();
void Home(void);
void Fade_out(void);
void SetXY(uint16_t x, uint16_t y);
