#include "binary.h"
#include "glyphs.h"

/* Alphanumeric 3x5 glyphs. Capital A-Z and 0-9 */
unsigned char g_alphaNumGlyphs[36][2] = {

	/*      010
		101
		111
		101
		101  A*/
	{ B01010111,
	  B11011010 }, 
	  
	/*	110
		101
		110
		101
		110  B*/
	{ B11010111,
	  B01011100 }, 
	  
	/*	011
		100
		100
		100
		011  C*/
	{ B01110010,
	  B01000110 },
	 
	/*	110
		101
		101
		101
		110  D*/
	{ B11010110,
	  B11011100 },
	  
	/*	111
		100
		110
		100
		111  E*/
	{ B11110011,
	  B01001110 },
	  
	/*	111
		100
		110
		100
		100  F*/
	{ B11110011,
	  B01001000 },
	  
	/*	011
		100
		101
		101
		011 G*/
	{ B01110010,
	  B11010110 },
	  
	/*	101
		101
		111
		101
		101  H*/
	{ B10110111,
	  B11011010 },
	  
	/*	111
		010
		010
		010
		111  I*/
	{ B11101001,
	  B00101110 },
	  
	/*	001
		001
		001
		101
		010  J*/
	{ B00100100,
	  B11010100 },
	  
	/*	101
		101
		110
		101
		101  K*/
	{ B10110111,
	  B01011010 },
	  
	/*	100
		100
		100
		100
		111  L*/
	{ B10010010,
	  B01001110 },
	  
	/*	101
		111
		101
		101
		101  M*/
	{ B10111110,
	  B11011010 },
	  
	/*	110
		101
		101
		101
		101  N*/
	{ B11010110,
	  B11011010 },
	  
	/*	010
		101
		101
		101
		010  O*/
	{ B01010110,
	  B11010100 },
	  
	/*	110
	  	101
	  	110
	  	100
	  	100  P*/
	{ B11010111,
	  B01001000 },
	  
	/*	010
		101
		101
		110
		011  Q*/
	{ B01010110,
	  B11100110 },
	  
	/*	110
		101
		110
		101
		101  R*/
	{ B11010111,
	  B01011010 },
	  
	/*	011
		100
		010
		001
		110  S*/
	{ B01110001,
	  B00011100 },
	  
	/*	111
		010
		010
		010
		010  T*/
	{ B11101001,
	  B00100100 },
	  
	 /*	101
	 	101
	 	101
	 	101
	 	111  U*/
	 { B10110110,
	   B11011110 },
	   
	 /*	101
	 	101
	 	101
	 	101
	 	010  V*/
	 { B10110110,
	   B11010100 },
	   
	 /*	101
	 	101
	 	101
	 	111
	 	101  W*/
	 { B10110110,
	   B11111010 },
	   
	 /*	101
	 	101
	 	010
	 	101
	 	101  X*/
	 { B10110101,
	   B01011010 },
	   
	 /*	101
	 	101
	 	010
	 	010
	 	010  Y*/
	 { B10110101,
	   B00100100 },
	   
	 /*	111
	 	001
	 	010
	 	100
	 	111  Z*/
	 { B11100101,
	   B01001110 },
	   
	 /*	111
	 	101
	 	101
	 	101
	 	111  0*/
	 { B11110110,
	   B11011110 },
	   
	 /*	010
	 	110
	 	010
	 	010
	 	111  1*/
	 { B01011001,
	   B00101110 },
	   
	 /*	110
	 	001
	 	010
	 	100
	 	111  2*/
	 { B11000101,
	   B01001110 },
	   
	 /*	110
	 	001
	 	010
	 	001
	 	110  3*/
	 { B11000101,
	   B00011100 },
	  
	 /*	101
	 	101
	 	111
	 	001
	 	001  4*/
	 { B10110111,
	   B10010010 },
	   
	 /*	111
	 	100
	 	110
	 	001
	 	110  5*/
	 { B11110011,
	   B00011100 },
	   
	 /*	111
	 	100
	 	111
	 	101
	 	111  6*/
	 { B11110011,
	   B11011110 },
	   
	 /*	111
	 	001
	 	010
	 	010
	 	010  7*/
	 { B11100101,
	   B00100100 },
	   
	 /*	111
	 	101
	 	111
	 	101
	 	111  8*/
	 { B11110111,
	   B11011110 },
	   
	 /*	111
	 	101
	 	111
	 	001
	 	001  9*/
	 { B11110111,
	   B10010010 }
};

unsigned char g_testGlyph = B11010100;
