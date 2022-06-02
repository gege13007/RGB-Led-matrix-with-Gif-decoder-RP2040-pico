#define EXTENSION_INTRODUCER   0x21
#define IMAGE_DESCRIPTOR       0x2C
#define TRAILER                0x3B

#define GRAPHIC_CONTROL        0xF9
#define APPLICATION_EXTENSION  0xFF
#define COMMENT_EXTENSION      0xFE
#define PLAINTEXT_EXTENSION    0x01

#define DICO_MAX_LEN		       4096

typedef struct {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} rgb;


typedef struct {
  int16_t code;
  uint16_t prev;
  uint8_t len;
} Entry_t;

//Decode
void pushpix(uint8_t);
void poppix(void); 
uint8_t getgif(void);
void Setpix(uint8_t);
void process_gif(void);




