#ifndef _motors_h_
#define _motors_h_

/*
 * Vozi oba motora
 * 
 * Input -100:100
 */
void motor( int accele_l, int accele_r );

/*
 * Vozi prednji servo
 * 
 * Input -45:45
 */
void handle( int angle );

#endif
