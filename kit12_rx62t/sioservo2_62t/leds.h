#ifndef _leds_h_
#define _leds_h_

/***************************************
 * Pali/gasi diode na mikrokontroleru
 **************************************/
void led_out_m( unsigned char led );


/****************************************
 * Pali/gasi diode na motor ploci:
 *		 0x01 - prva dioda  // prvi bit
 *		 0x02 - druga dioda // drugi bit
 *		 0x03 - obe
 ***************************************/
 
void led_out( unsigned char led );

#endif
