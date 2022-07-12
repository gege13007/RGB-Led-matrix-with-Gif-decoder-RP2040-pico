#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include "matrix.h"
#include "gif.h"

static uint32_t ptrfile=0;

//Canvas Gif décompressé (en code de couleurs)
static uint8_t pix[nb_pixel_X * nb_pixel_Y];

//Table color globale (FIXE)
static rgb coltab[256*3];

static Entry_t dico[DICO_MAX_LEN];

static uint16_t pixwidth, pixheight, glob_coltab_size; 
static uint8_t bgcolindex, glob_coltab_ok, pixratio;
static uint32_t maxpixels;

static uint16_t clipleft, cliptop, clipwidth, clipheight;
static uint8_t field;
//Lobal color table
static uint16_t loc_coltab_size=0;
//Flag si Interlacé
static uint8_t interlace=0;
//N fois display Gif anim frame (0 -> loop)
static uint8_t display_times=0;
//Délai en ms par frame
static uint16_t display_ms, frame_num=0;
//Couleur si transparence
static uint8_t display_transpa=0, display_trans_col=0;
//Disposal mode
static uint8_t display_disposal;

//Pour decoder
static uint8_t initablesize, tablesize, mask;
static uint16_t MSBbit, maxdicoptr, clearcode, endcode, outptr;
static uint16_t clipx = 0;
static uint8_t packleft = 0, newx=0;
static uint16_t dicoptr=0, gifptr=0;
static uint16_t newb9=0; 
static int16_t oldcode;

//pour la pile de pixels (ordre à inverser)
//(premiers entrés -> derniers setpix)
static uint16_t pixstackptr=0;
static uint8_t pixstack[320];

// Lecture indrecte du tableau picture.h
uint8_t getgif(uint8_t tab[]) {
// if ( ptrfile >= sizeof(pixgif) )
//   return(TRAILER);
// else  
  return(tab[ptrfile++]);
}


//---------------------------------------------
//        LOGICAL  SCREEN  DESCRIPTOR (7)
//---------------------------------------------
int process_header(uint8_t tab[]) {
uint8_t x;

 x = getgif(tab);
 if (x!=0x47) exit(0);    // G
 x = getgif(tab);
 if (x!=0x49) exit(0);    // I
 x = getgif(tab);
 if (x!=0x46) exit(0);    // F

 for (uint8_t nn=0; nn<3 ; nn++) {
   x = getgif(tab);
 }
 return 1;
}


//---------------------------------------------
//       LOGICAL  SCREEN  DESCRIPTOR (7)
//---------------------------------------------
void process_logical_descriptor(uint8_t tab[]) {

 pixwidth = getgif(tab) + 256 * getgif(tab);
 pixheight = getgif(tab) + 256 * getgif(tab);
 
 //redim le canvas du Gif (logique et pas Leds physique)
 maxpixels = pixwidth * pixheight;
 //pix = (uint8_t*) malloc(maxpixels);

 uint8_t field = getgif(tab);
 
 //Test Global color table ?
 glob_coltab_size = 1 << ((field & 7) + 1);
 glob_coltab_ok = (field & 128);
   
 bgcolindex = getgif(tab);
 pixratio = getgif(tab);
}


//---------------------------------------------
//           IMAGE   DESCRIPTOR (9)
//---------------------------------------------
void process_image_descriptor(uint8_t tab[]) {

  clipleft = getgif(tab) + 256 * getgif(tab);   // clip left pos
  cliptop = getgif(tab) + 256 * getgif(tab);    // clip top pos
  clipwidth = getgif(tab) + 256 * getgif(tab);
  clipheight = getgif(tab) + 256 * getgif(tab);
  
  uint8_t f = getgif(tab);
  
  //Lobal color table ?
  loc_coltab_size = 1 << ((f & 7) + 1);
  interlace = (f & 64);
}

//---------------------------------------------
//    APPLICATION (NETSCAPE) EXTENSION (11+)
//---------------------------------------------
void process_appli_extension(uint8_t tab[]) {
uint8_t a,b,c1,c2,c3,x;

  b = getgif(tab);
  //capte les 3 premiers chars pour test
  //soit 'NETSCAPE' (gifanim) soit 'XMP' (???)
  c1 = getgif(tab);
  c2 = getgif(tab);
  c3 = getgif(tab);
  for (a = 1; a < b-3; a++) x=getgif(tab);
  
  //Si NETSCAPE (gifanim)
  if ( (c1=='N') && (c2=='E') && (c3=='T') ) {
    a = getgif(tab);  // must be = 3
    b = getgif(tab);  // must be = 1
    display_times = getgif(tab);
    display_times = display_times + 256 * getgif(tab);  // n fois loops (0 = infini)
  }
  else
    //Si XMP vide ???
    while (1) {
      if (x==0) break;
      x = getgif(tab);
    }
}


//----------------------------------------------
// Graphics Control extension (Anim & Transpar)
//----------------------------------------------
void process_control_extension(uint8_t tab[]) {
uint8_t a,b;

  //len of data sub-block
  a = getgif(tab);       // must be = 4 (byte size)
  b = getgif(tab);       // packed fields - graphics disposal
  
  // >0 si color transparence
  display_transpa = b & 1;
  
  // 01 (dont dispose / graphic), 02 (overwrite graph / background color), 04 (overwrite graphic with previous graphic)
  display_disposal = (b & 0x1C) >> 2;
  display_ms = getgif(tab);
  display_ms = display_ms + 256 * getgif(tab);  // ms / frames
  
  //color transparence index
  display_trans_col = getgif(tab);              // Transpar Color Index
}     


//---------------------------------------------------
// Lit les octets du .gif et passe en code (newb9) de
// longueur variable ...de 8 à 9 bits (ou plus...)
// Gère les paquets d'octets (packleft)
//---------------------------------------------------
//Sort si packleft=0 : plus de paquets à lire (getCode = endcode)
//Sort si fin du fichier dépassée (getCode = endcode)
uint16_t getCode(uint8_t tab8[]) {

 //enquille les 9 bits - rentre sur le 8 - shift vers droite
 for (uint8_t nn = 0; nn < tablesize; nn++) {
   //shift droit mot 9 bits destination
   newb9 = newb9 >> 1;
   //met MSB dans b9 précédent
   if ((newx & 1) > 0) newb9 = newb9 + MSBbit;
   //shift droit l'octet en cours de lecture
   newx = newx >> 1;
   mask = mask - 1;
   //un code (newb9) est-il prêt ?
   if (mask == 0) {
     //nouveau pack de 8 bits (ou moins ?)
     mask = 8;
     //Test si charger nouveau paquet de gif bytes ?
     if (packleft == 0) {
       packleft = getgif(tab8);
       //POUR SECURITE - Test si sort du Tableau ?
       if (gifptr > sizeof(tab8)) return(endcode);
       //test si plus de paquets picture datas
       if (packleft == 0) return(endcode);
     }
     newx = getgif(tab8);
     packleft = packleft - 1;
   }
 }
 return(newb9);
}

//Système de pile pour inverser le sens d'envoi des pixels
// (les pixels sont décodés et arrivent (PUSH) comme 3.2.1 et doivent
// être affichés (POP) comme 1.2.3 !)
void pushpix(uint8_t z) {
  pixstack[pixstackptr] = z;
  pixstackptr = pixstackptr + 1;
}

void poppix(void) {
 while (1) {
   pixstackptr = pixstackptr - 1;
   Setpix(pixstack[pixstackptr]);
   if (pixstackptr < 1) break;
 }
}

//Store un pixel dans pix[...] avec couleur z
void Setpix(uint8_t z) {
 //Store un Pixel / sauf si pixel en transparence
 if (outptr > maxpixels) return;
  
 pix[outptr] = z;
 
 //Si le Clip + petit quePix -> test si retour ligne ?
 clipx = clipx + 1;
 if (clipx >= clipwidth) {
   clipx = 0;
   outptr = outptr + pixwidth - clipwidth;
 }
 outptr = outptr + 1;
}

void pset2(uint16_t x, uint16_t y, uint8_t col) {
  // Case 0, 1
  if (display_disposal < 2) {
    //Si dans le nouveau clip ?
    if (x < clipleft) return;
    if (y < cliptop) return;
    if (x >= clipleft + clipwidth) return;
    if (y >= cliptop + clipheight) return;    
    
    //test si draw ou transparent
    if ((col != display_trans_col) || (display_transpa == 0))
      Pset(x, y, coltab[col].r, coltab[col].g, coltab[col].b);
      
  } else { 
  // Case 2 To 7
  //si disposal to bground / met fond gris forcé sur transparence
  //englobe aussi le disposal mode 3 (?)
   if ((col == display_trans_col) && (display_transpa > 0))
     Pset(x, y, 1, 0, 0);
   else
     Pset(x, y, coltab[col].r, coltab[col].g, coltab[col].b);
  }
}

//--------------------------------------------------------
//             Dessine de l'Image en Clair
//--------------------------------------------------------
void drawpix(void) {
uint16_t pt=0;
uint16_t a, b;
uint8_t col;
 
if (interlace > 0) {
  // Interlacé - passe 1
  for (b = 0, pt=0; b < pixheight; b+=8) {
    for (a = 0; a < pixwidth; a++) {
      col = pix[pt++];
      pset2(a, b, col);
    }
  }
  //passe 2
  for (b = 4; b < pixheight; b+=8) {
    for (a = 0; a < pixwidth; a++) {
      col = pix[pt++];
      pset2(a, b, col);
    }
  }
  //passe 3
  for (b = 2; b < pixheight; b+=4) {
    for (a = 0; a < pixwidth; a++) {
      col = pix[pt++];
      pset2(a, b, col);
    }
  }
  //passe 4
  for (b = 1; b < pixheight; b+=2) {
    for (a = 0; a < pixwidth; a++) {
      col = pix[pt++];
      pset2(a, b, col);
    }
  }
 } else {
  //Non Interlacé
  for (b = 0,pt=0; b < pixheight; b++) {
    for (a = 0; a < pixwidth; a++) {
      // les pixels sont à la suite dans pix[...]
      col = pix[pt++];
      pset2(a, b, col);
    }
  }
 }
}


//Raz le dico / clearcode doit etre réglé
void Raz_dico(uint8_t tab[]) {
uint16_t a, b;

 tablesize = initablesize;
 
 for (a = 0; a < DICO_MAX_LEN; a++) {
   dico[a].code = -1;
   dico[a].len = 0;
   dico[a].prev = 0;
 }
 
 for (a = 0; a < clearcode; a++) {
   dico[a].code = a;
 }
 
 dicoptr = 1 + endcode;
 newb9 = getCode(tab);
}


/****************************************************************
 * @param gif_file the file descriptor of a file
 *  GIF-encoded file
 * 
 ****************************************************************/
void process_gif(uint8_t tab[]) {
uint8_t ext, x;
uint16_t a, b, i;
uint16_t pt=0;
 
 //Raz du Gif
 ptrfile=0;
 
 //Raz ptr pile des pixels en sortie
 pixstackptr=0;
 
 // A GIF file starts with a Header GIF
 if (process_header(tab)<0) return;
 
 //Logical desc
 process_logical_descriptor(tab);

 // Load GLOBAL COLOR TABLE
 if (glob_coltab_ok > 0) {
   for (i=0; i < glob_coltab_size; i++) {
     // static rgb *glob_coltab;
     coltab[i].r = getgif(tab);
     coltab[i].g = getgif(tab);
     coltab[i].b = getgif(tab);
   }
 }
    
 // Next Frame ...
 while (1) {

 // Analyse les Extensions
 while (1) {
   ext = getgif(tab);
   while (1) {
     if (ext>0) break; 
     ext=getgif(tab);
   }
   
   //Si Fin ?
   if (ext == TRAILER) {
     break;               // == 3B
   }
   
   //Si Extension Introducer ?
   if (ext != EXTENSION_INTRODUCER) break;  // != 21

   //Traite next Extension
   ext = getgif(tab);
  
   switch (ext) {
   case APPLICATION_EXTENSION:       // FF
     process_appli_extension(tab);
     break;

   case GRAPHIC_CONTROL:             // F9
     process_control_extension(tab);
     break;
     
   default:
     b = getgif(tab);                           // comments ou autres...
     for (a = 0; a < b; a++) x = getgif(tab);   // !!!!! A VOIR !!!!!
   }
   
   //Fin de bloc extension = 0 !
   ext = getgif(tab);
  }
  
  // Si Fin
  if (ext == TRAILER) {
    break;               // == 3B
  }
  
  // Sinon Image Descriptor (Doit etre 2C)
  if (ext != IMAGE_DESCRIPTOR) break;

  //Donne les clip/left/top/width/interlace/local coltab...
  process_image_descriptor(tab);
  
  // LOCAL COLOR TABLE ?
  if (loc_coltab_size > 4) {
    for (i=0; i < loc_coltab_size; i++) {
      // static rgb *glob_coltab;
      coltab[i].r = getgif(tab);
      coltab[i].g = getgif(tab);
      coltab[i].b = getgif(tab);
    }
  }
  
//---------------------------------------------
//           I M A G E     D A T A S
//---------------------------------------------
 initablesize = (1 + getgif(tab));      // 9 bits au debut
 tablesize = initablesize;
 MSBbit = 1 << (initablesize - 1);
 maxdicoptr = (1 << tablesize) - 1;
 clearcode = 1 << (tablesize - 1);
 endcode = 1 + clearcode;
   
 //Init ptr pix
 outptr = clipleft + cliptop * pixwidth;
 clipx = 0;
 
 //Lecture par paquets de 'byteslong' <255
 //si packleft=0 -> la première lecture est une taille de bloc !
 packleft = 0;
 mask = initablesize;
 
 // A VOIR ! A VOIR !
/* if (display_disposal == 2) {
   for (b = 0,pt=0; b < pixheight; b++) {
     for (a = 0; a < pixwidth; a++) {
       // les pixels sont à la suite dans pix[...]
       pix[pt++]=bgcolindex;
     }
   }
 }  */

//-------------------------------------------------
//                   DECODE  L Z W
//-------------------------------------------------
 Raz_dico(tab);
 
 while (1) {
   newb9 = getCode(tab);
   
   if (newb9 == clearcode) {
     tablesize = initablesize;
     MSBbit = 1 << (initablesize - 1);
     maxdicoptr = (1 << tablesize) - 1;
     Raz_dico(tab);
     oldcode=newb9;
     newb9 = getCode(tab);
	   Setpix(dico[oldcode].code);
   }
   
  // Increase Size -> GIF89a mandates that this stops at 12 bits
  if (dicoptr == maxdicoptr) {
    if (tablesize < 12) {
      tablesize = tablesize + 1;
      MSBbit = 1 << (tablesize - 1);
      maxdicoptr = (1 << tablesize) - 1;
    }
  }
  
  if (newb9 == endcode) break;     // Si Fin image ?
     
  //YES code is in the code table
  if (dico[newb9].code > -1) {
    a = dico[newb9].len;
    b = newb9;
    while (1) {
      pushpix(dico[b].code);
      b = dico[b].prev;
      if ( (b > 0) && (b < clearcode) ) {
        pushpix(b);
        break;
	    }
      if (a == 0) break;
      a = a - 1;
    }
    //affiche les pixels dans l'ordre inverse
    poppix();
     
  } else {
    
    //Not in DICO : out {code-1}+K to stream
    a = dico[oldcode].len;
    b = oldcode;
    pushpix(dico[b].code);
    while (1) {
      pushpix(dico[b].code);
      b = dico[b].prev;
      if ( (b > 0) && (b < clearcode) ) {
        pushpix(b);
        break;
      }
      if (a == 0) break;
      a = a - 1;
    }
    //affiche les pixels dans l'ordre inverse
    poppix();
  }
  
  //ADD to DICO S+first symbol / {code-1}+K
  dico[dicoptr].prev = oldcode;
  dico[dicoptr].len = dico[oldcode].len + 1;
  
  //cherche le premier char(code) en récursif si long>1 avec .prev
  b = newb9;
  if (b < clearcode)
    dico[dicoptr].code = dico[b].code;
  else {
   while (1) {
     a = dico[b].len;
     if (a > 1)
       b = dico[b].prev;
     else {
       dico[dicoptr].code = dico[b].prev;
       break;
     }
   }
  }
  
  if (dicoptr < 4096) dicoptr++;
  
  oldcode = newb9;
 }

 drawpix();
 
 frame_num++;
 
 //DEBUG
 // SetXY(48, 1);
 // PrintHex(frame_num++);

  if (display_ms<2) display_ms=20;
  if (display_ms>19999) display_ms=8000;

  sleep_ms(display_ms);
 
 //end frame -> next ...
 }
 
}
