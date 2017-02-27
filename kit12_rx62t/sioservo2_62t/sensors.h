#ifndef _sensors_h_
#define _sensors_h_

#define USE_INVERTED_LOGIC 1 // 0 for standard logic, 1 for inverted

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

#define CROSSLINE		0xff 			/* X X X X  X X X X            */
#define RIGHTLINE		0x1f		  	/* O O O X  X X X X            */
#define LEFTLINE		0xf8            /* X X X X  X O O O            */
#define NOLINE			0x00            /* 0 0 0 0  0 O O O            */

unsigned char sensor_inp( unsigned char mask );

unsigned char startbar_get( void );

int check_crossline( void );
int check_rightline( void );
int check_leftline( void );
int check_noline( void );

#endif
