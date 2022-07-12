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
// Longueur Totale des MATRICES en série (sur une seule ligne)
static uint8_t nMatrix_X = 6;

//Nb de panneaux sur vrai largeur (mapping des panneaux en ZStripe = zigzag) de x large
static uint8_t zStripes_X = 6;

// Nombre de LEDS par Matrices
static uint8_t nLedsMatrix_Y = 32;
static uint8_t nLedsMatrix_X = 32;

// - - - - Description LOGIQUE de L'écran - - - - - - 
// Dimensions du Buffer logique BITMAP en PIXELS X & Y
// la limite en RAM est proche de 164 ko sur 264ko dispo.
// * par exemple 320 * 128 pixels -> 4 x 40960 oc.
// * peut être supérieur a l'afficheur physique
// * buffer utilisé pour fichiers gif
#define nb_pixel_X        256     // donne 4 x 61440 oc. = 33 ko / 264ko de RAM
#define nb_pixel_Y        32

static uint8_t xRED[nb_pixel_X][nb_pixel_Y] = {0};
static uint8_t xGREEN[nb_pixel_X][nb_pixel_Y] = {0};
static uint8_t xBLUE[nb_pixel_X][nb_pixel_Y] = {0};

//Curseur next PrintChar
static uint16_t CurX=0;
static uint16_t CurY=0; 
 
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
