#ifndef _sensors_h_
#define _sensors_h_

/* MAKRO ZA LOGIKU SENZORA
 * 0 - for standard logic 
 * 1 - for inverted
 */
#define USE_INVERTED_LOGIC 1 

/* Masked value settings X:masked (disabled) O:not masked (enabled) */
#define MASK2_2         0x66            /* X O O X  X O O X            */
#define MASK2_0         0x60            /* X O O X  X X X X            */
#define MASK0_2         0x06            /* X X X X  X O O X            */
#define MASK3_3         0xe7            /* O O O X  X O O O            */
#define MASK0_3         0x07            /* X X X X  X O O O            */
#define MASK3_0         0xe0            /* O O O X  X X X X            */
#define MASK4_0         0xf0            /* O O O O  X X X X            */
#define MASK0_4         0x0f            /* X X X X  O O O O            */
#define MASK4_4         0xff            /* O O O O  O O O O            */

#define MASK4_1         0xf8            /* O O O O  O X X X            */
#define MASK1_4         0x1f            /* X X X O  O O O O            */

#define CROSSLINE		0xff 			/* X X X X  X X X X            */
#define RIGHTLINE1		0x1f		  	/* O O O X  X X X X            */
#define LEFTLINE1		0xf8            /* X X X X  X O O O            */
#define RIGHTLINE2		0x0f		  	/* O O O O  X X X X            */
#define LEFTLINE2		0xf0            /* X X X X  O O O O            */
#define NOLINE			0x00            /* 0 0 0 0  0 O O O            */
#define STRAIGHTLINE    0x18			/* 0 0 0 X  X O O O            */


/*
 * funkcija koja proverava stanje dioda maskiranih sa prosledjenom maskom
 * 
 * Output - stanje maskranih senzora
 * 			0x00 : 0xff
 */
unsigned char sensor_inp( unsigned char mask );


/*
 * Posmatra stanje startbar senzora
 *
 * Output: ON/OFF
 */
unsigned char startbar_get( void );


/*
 * Dtektuje crossline - pred lakat krivine
 *
 * Output: ON/OFF
 */
int check_crossline( void );

/*
 * Detektuje RIGHTLINE1 ili RIGHTLINE2
 *
 * Output: ON/OFF
 */
int check_rightline( void );

/*
 * Detektuje LEFTLINE1 ili LEFTTLINE2
 *
 * Output: ON/OFF
 */
int check_leftline( void );


/*
 * Proverava kada ne postoji linije
 *
 * Output: ON/OFF
 */
int check_noline( void );

/*
 * Detektuje centralnu liniju
 *
 * Output: ON/OFF
 */
int check_noline( void );

#endif
