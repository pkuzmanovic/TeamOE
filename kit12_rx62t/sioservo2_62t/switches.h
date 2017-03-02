#ifndef _switches_h_
#define _switches_h_

/*
 * Ocitava vrednost deepSwitcha
 *
 * Output: 0:15
 */
unsigned char dipsw_get( void );

/*
 * Ocitava vrednost buttona na mikrokontroleru
 *
 * Output: ON/OFF
 */
unsigned char buttonsw_get( void );

/*
 * Ocitava vrednost buttona na motorploci
 *
 * Output: ON/OFF
 */
unsigned char pushsw_get( void );



#endif
