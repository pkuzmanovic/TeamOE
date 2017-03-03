#include "leds.h"
#include "motors.h"
#include "sensors.h"
#include "switches.h"

#define K 1

extern unsigned long cnt0;
extern unsigned long cnt1;
extern int pattern;

extern volatile int accele_l;
extern volatile int accele_r;
extern volatile int angle;

extern volatile int sensor_filtered;

int BROJAC = 0;
void drive();

void process_1() { // ours

	switch( pattern ) {
		/****************************************************************
        Pattern-related
         00: wait for switch input
		 01: check if start bar is open
         10: drive
		 20 - 39 : elbow chicane
		 40 - 59 : change lane left
		 60 - x  : change lane right
        ****************************************************************/
		case 00:
			/* Wait for switch input */
            if( pushsw_get() ) {
                //pattern = 01;  // uncomment if race start
				pattern = 10; // straight to 10 for now, no startbar checking
				led_out( 0x0 );
                cnt0 = 0;
                break;
            }
			
			angle = 0;
			accele_l = 0;
			accele_r = 0;
            
			if( cnt0 < 100 ) {          // LED flashing processing
                led_out( 0x1 );
            } else if( cnt0 < 200 ) {
                led_out( 0x2 );
            } else {
                cnt0 = 0;
            }
			
			// code for measuring time
			/*BROJAC++;  
			if (BROJAC < 10000000) led_out(0x1);
			else if (BROJAC < 20000000) led_out(0x0);
			else BROJAC = 0;*/
			
            break;
			
		case 01:
            /* Check if start bar is open */
            if( !startbar_get() ) {
                /* Start!! */
                led_out( 0x0 );
                pattern = 10;
                cnt0 = 0;
                break;
            }
            if( cnt0 < 50 ) {           /* LED flashing processing     */
                led_out( 0x1 );
            } else if( cnt0 < 100 ) {
                led_out( 0x2 );
            } else {
                cnt0 = 0;
            }
            break;
			
	/*******************************************************************************************************************************
	*********************************************** DRIVE 10 ***********************************************************************
	********************************************************************************************************************************/	
			
			
		case 10:     // regulate angle and speed to follow the curve (no crosslines)
			     	
			led_out( 0x3 );
			
			/* Drive function */
			drive();
			
			/* Next state logic */
			if( check_crossline() ) {   // Cross line check            
                pattern = 20;  // elbow shikana
                break;
            }
            if( check_leftline() ) {    // Left half line detection check
                pattern = 40;  // change lane left
                break;
            }
			if( check_rightline() ) {   // Right half line detection check
                pattern = 60;  // change lane right
                break;
            }
			/* ( check_noline() ) {
				// stop
				pattern = 00;
				angle = 0;
				accele_l = 0;
				accele_r = 0;
				break;
			}*/
				
		   break;
	
	/*******************************************************************************************************************************
	*********************************************** ELBOW CHICANE 20-39 *************************************************************
	********************************************************************************************************************************/		
				  
		case 20: // first CROOSSLINE happened
			/* elbow chicane */
	
			// turn off both diodes
			led_out( 0x0 );
			
			if (check_straightline())
				pattern = 21;
			break;
		
		case 21: // first STRAIGHTLINE happened
			
			if (check_crossline())
				pattern = 22;
			break;
		
		case 22: // second CROSSLINE happened
			
			if (check_straightline())
				pattern = 23;
			break;
		
		case 23:  // slow down, check left/right chicane
			
			accele_l = 50;
			accele_r = 50;
			
			/* fina podesavanja nosa */
			switch( sensor_filtered ) {
				
			    // straghtline
               case 0x18:
                   angle = 0;				   
				   break;
				
				// skreni malo udesno
			   case 0x0c:
                   angle = 8;
				   break;
				   
               case 0x08:
                   angle = 6;
				   break;
				
			   case 0x1c:
                   angle = 4;
				   break;
				   
				// skreni malo ulevo
			   case 0x30:
                   angle = -8;
                   break;
				   
			   case 0x10:
                   angle = -6;
                   break;
				   
               case 0x38:
                   angle = -4;
                   break;
				default:
					angle = 0;
					break;
			}
			
			if ( check_leftline() ) {
				pattern = 24;  // left chicane
				break;
			}
			if ( check_rightline() ) {
				pattern = 26;  // right chicane
				break;
			}
			
			break;

		
		case 24: // left chicane
		
			angle = 0;
			accele_l = 50;
			accele_r = 50;
			
			if (check_noline()) {
				pattern = 25;
			}
			break;
		
		case 25: // TURN 90, left!
			accele_l = 20;
			accele_r = 90;
			angle = -40;
		
			if ( ( sensor_filtered & 0x30) == 0x30 ) {
				pattern = 10;
			}
			break;
			
		case 26: // right chicane
		
			angle = 0;
			accele_l = 50;
			accele_r = 50;
			
			if (check_noline()) {
				pattern = 27;
			}
			break;
			
		case 27: // TURN 90, right!
			accele_l = 90;
			accele_r = 20;
			angle = 40;
					
			if ( (sensor_filtered & 0x0c) == 0x0c ) {
				pattern = 10;
			}
			break;
			
	/*******************************************************************************************************************************
	********************************************* LEFT CHANGE LANE 40-59 ***********************************************************
	********************************************************************************************************************************/		
				
				
		case 40: // first LEFTLINE happened
		
			led_out( 0x2 );
			
			if ( check_straightline() )
				pattern = 41;
			break;
			
		case 41: // second STRAIGHTLINE happened

			if (check_leftline())
				pattern = 42;
			break;
		
		case 42:  // second LEFTLINE happened

			if (check_straightline())
				pattern = 43;
			break;
		
		case 43:  // CHANGE LINE, LEFT
			
			accele_l = 60;
			accele_r = 60;
			
			if ( check_noline() ) { // all black
				pattern = 44;
			}
			break;
			
		case 44: // first angle change
		
			angle = -30;
			accele_l = 30;
			accele_r = 70;

			if (sensor_inp(0x18) == 0x18) { // wait for the 2 central LEDs
				pattern = 45;
			}
			break;
			
		case 45: // second angle change
		
			angle = 15;
			accele_l = 70;
			accele_r = 50; 

			if (sensor_inp(MASK3_3) == 0x00) { // wait for the formula to catch the line properly, then go back to pattern 10
				pattern = 10;
			}
			break;
			
	/*******************************************************************************************************************************
	******************************************** RIGHT CHANGE LANE 60 - x **********************************************************
	********************************************************************************************************************************/		
			
		case 60: // first RIGHTLINE happened
			led_out( 0x1 );
			if (check_straightline())
				pattern = 61;
			break;
			
		case 61: // second STRAIGHTLINE happened
			if (check_rightline())
				pattern = 62;
			break;
		
		case 62: // second RIGHTLINE happened 
			if (check_straightline())
				pattern = 63;
			break;
		
		case 63: // CHANGE LINE, RIGHT
			// slow down (later, make it slow down gradually, not immediately)
			accele_l = 60;
			accele_r = 60;
			
			// CHANGE LANE, RIGHT
			if ( check_noline() ) { 
				pattern = 64;
			}
			break;
			
		case 64: // first angle change
			angle = 30;
			accele_l = 70;
			accele_r = 30;
			
			if (sensor_inp(0x18) == 0x18) { // wait for the 2 central LEDs
				pattern = 65;
			}
			break;
			
		case 65: // second angle change

			angle = -15;
			accele_l = 50; 
			accele_r = 70;
			
			if (sensor_inp(MASK3_3) == 0x00) { // wait for the formula to catch the line properly, then go back to pattern 10
				pattern = 10;
			}
			break;	
		
		default:
		   break;
	}
}






/*
 * Drive function
 */
void drive(){
	switch( sensor_filtered ) {
           case 0x18:
               angle = 0;
			   accele_l = 100;
			   accele_r = 100;
			   break;
			
		   // skreni malo udesno
		   case 0x0c:
               angle = 21*K;
			   accele_l = 100;
			   accele_r = 90;
			   break;
			   
           case 0x08:
               angle = 18*K;
			   accele_l = 100;
			   accele_r = 90;
			   break;
			
		   case 0x1c:
               angle = 15*K;
			   accele_l = 100;
			   accele_r = 90;
			   break;
			   
		   // skreni malo ulevo
		
		   case 0x30:
               angle = -21;
			   accele_l = 90;
			   accele_r = 100;
               break;
			   
		   case 0x10:
               angle = -18;
			   accele_l = 90;
			   accele_r = 100;
               break;
			   
           case 0x38:
               angle = -15;
			   accele_l = 90;
			   accele_r = 100;
               break;
			   
		   // skreni malo vise udesno

           case 0x06:
		   	   angle = 30;
			   accele_l = 80;
			   accele_r = 67;
			   break;
			   
		   case 0x04:
               angle = 25;
			   accele_l = 80;
			   accele_r = 67;
			   break;
			   
		   case 0x0e:
               angle = 22;
			   accele_l = 80;
			   accele_r = 67;
			   break;
			   
		   // skreni malo vise ulevo
		   case 0x60:
		       angle = -30;
			   accele_l = 67;
			   accele_r = 80;
               break;
			   
		   case 0x20:
		       angle = -25;
			   accele_l = 67;
			   accele_r = 80;
               break;
			   
		   case 0x70:
               angle = -22;
			   accele_l = 67;
			   accele_r = 80;
               break;
			   
		   // bas skreni udesno
           case 0x07:
		   	   angle = 30;
			   accele_l = 50;
			   accele_r = 38;
			   break;
		   
		   case 0x02:
		   	   angle = 30;
			   accele_l = 50;
			   accele_r = 38;
			   break;
			   
		   // bas skreni ulevo
		   case 0xe0:
		    	angle = -30;
				accele_l = 38;
			  	accele_r = 50;
                break;
		  
		   case 0x40:
		   	   angle = -30;
			   accele_l = 38;
			   accele_r = 50;
			   break;
			   
		   // krajnji slucajevi
			   
		   case 0x03:
		   	   // otisao previse levo od centra, skreni desno
		   	   angle = 40;
			   accele_l = 50;
			   accele_r = 38;
			   break;
		  
		  	case 0xc0:
		   		// otisao previse desno od centra, skreni levo
		   		angle = -40;
				accele_l = 38;
			    accele_r = 50;
			    break;
				
				
		  /* case 0x01:
		   	   // otisao previse desno od centra, skreni levo (?)
		   	   angle = 40;
			   accele_l = 38;
			   accele_r = 50;
			   break;*/
			   		   	   

		   
		   /*case 0x80:
		   		// otisao previse levo od centra, skreni desno (?)
		   		angle = -40;
				accele_l = 50;
			    accele_r = 38;
			    break;*/
				
		   // stop cases
		   case 0x00: 
		   	   // stop
		   	   angle = 0;
			   accele_l = 0;
			   accele_r = 0;
			   break;
			   
		   default :
		   	   // nothing smarter than stop
		   	   //angle = 0;
			   //accele_l = 0;
			   //accele_r = 0;
			   break;
			 
	}		
}






// NJI'OVO


void process_0() { // default
	switch( pattern ) {

        /****************************************************************
        Pattern-related
         0: wait for switch input
         1: check if start bar is open
        11: normal trace
        12: check end of large turn to right
        13: check end of large turn to left
        21: processing at 1st cross line
        22: read but ignore 2nd time
        23: trace, crank detection after cross line
        31: left crank clearing processing ? wait until stable
        32: left crank clearing processing ? check end of turn
        41: right crank clearing processing ? wait until stable
        42: right crank clearing processing ? check end of turn
        51: processing at 1st right half line detection
        52: read but ignore 2nd line
        53: trace after right half line detection
        54: right lane change end check
        61: processing at 1st left half line detection
        62: read but ignore 2nd line
        63: trace after left half line detection
        64: left lane change end check
        ****************************************************************/


        case 0:
            /* Wait for switch input */
            if( pushsw_get() ) {
                pattern = 1;
                cnt1 = 0;
                break;
            }
            /*if( cnt1 < 100 ) {          // LED flashing processing
                led_out( 0x1 );
            } else if( cnt1 < 200 ) {
                led_out( 0x2 );
            } else {
                cnt1 = 0;
            }*/
			
		
			
            break;

        case 1:
            /* Check if start bar is open */
            if( !startbar_get() ) {
                /* Start!! */
                led_out( 0x0 );
                pattern = 11;
                cnt1 = 0;
                break;
            }
            if( cnt1 < 50 ) {           /* LED flashing processing     */
                led_out( 0x1 );
            } else if( cnt1 < 100 ) {
                led_out( 0x2 );
            } else {
                cnt1 = 0;
            }
            break;

        case 11:
            /* Normal trace */
            if( check_crossline() ) {   /* Cross line check            */
                pattern = 21;
                break;
            }
            if( check_rightline() ) {   /* Right half line detection check */
                pattern = 51;
                break;
            }
            if( check_leftline() ) {    /* Left half line detection check */
                pattern = 61;
                break;
            }
            switch( sensor_inp(MASK3_3) ) {
                case 0x00:
                    /* Center -> straight */
                    handle( 0 );
                    motor( 100 ,100 );
                    break;

                case 0x04:
                    /* Slight amount left of center -> slight turn to right */
                    handle( 5 );
                    motor( 100 ,100 );
                    break;

                case 0x06:
                    /* Small amount left of center -> small turn to right */
                    handle( 10 );
                    motor( 80 ,67 );
                    break;

                case 0x07:
                    /* Medium amount left of center -> medium turn to right */
                    handle( 15 );
                    motor( 50 ,38 );
                    break;

                case 0x03:
                    /* Large amount left of center -> large turn to right */
                    handle( 25 );
                    motor( 30 ,19 );
                    pattern = 12;
                    break;

                case 0x20:
                    /* Slight amount right of center -> slight turn to left */
                    handle( -5 );
                    motor( 100 ,100 );
                    break;

                case 0x60:
                    /* Small amount right of center -> small turn to left */
                    handle( -10 );
                    motor( 67 ,80 );
                    break;

                case 0xe0:
                    /* Medium amount right of center -> medium turn to left */
                    handle( -15 );
                    motor( 38 ,50 );
                    break;

                case 0xc0:
                    /* Large amount right of center -> large turn to left */
                    handle( -25 );
                    motor( 19 ,30 );
                    pattern = 13;
                    break;

                default:
                    break;
            }
            break;

        case 12:
            /* Check end of large turn to right */
            if( check_crossline() ) {   /* Cross line check during large turn */
                pattern = 21;
                break;
            }
            if( check_rightline() ) {   /* Right half line detection check */
                pattern = 51;
                break;
            }
            if( check_leftline() ) {    /* Left half line detection check */
                pattern = 61;
                break;
            }
            if( sensor_inp(MASK3_3) == 0x06 ) {
                pattern = 11;
            }
            break;

        case 13:
            /* Check end of large turn to left */
            if( check_crossline() ) {   /* Cross line check during large turn */
                pattern = 21;
                break;
            }
            if( check_rightline() ) {   /* Right half line detection check */
                pattern = 51;
                break;
            }
            if( check_leftline() ) {    /* Left half line detection check */
                pattern = 61;
                break;
            }
            if( sensor_inp(MASK3_3) == 0x60 ) {
                pattern = 11;
            }
            break;

        case 21:
            /* Processing at 1st cross line */
            led_out( 0x3 );
            handle( 0 );
            motor( 0 ,0 );
            pattern = 22;
            cnt1 = 0;
            break;

        case 22:
            /* Read but ignore 2nd line */
            if( cnt1 > 100 ){
                pattern = 23;
                cnt1 = 0;
            }
            break;

        case 23:
            /* Trace, crank detection after cross line */
            if( sensor_inp(MASK4_4)==0xf8 ) {
                /* Left crank determined -> to left crank clearing processing */
                led_out( 0x1 );
                handle( -38 );
                motor( 10 ,50 );
                pattern = 31;
                cnt1 = 0;
                break;
            }
            if( sensor_inp(MASK4_4)==0x1f ) {
                /* Right crank determined -> to right crank clearing processing */
                led_out( 0x2 );
                handle( 38 );
                motor( 50 ,10 );
                pattern = 41;
                cnt1 = 0;
                break;
            }
            switch( sensor_inp(MASK3_3) ) {
                case 0x00:
                    /* Center -> straight */
                    handle( 0 );
                    motor( 40 ,40 );
                    break;
                case 0x04:
                case 0x06:
                case 0x07:
                case 0x03:
                    /* Left of center -> turn to right */
                    handle( 8 );
                    motor( 40 ,35 );
                    break;
                case 0x20:
                case 0x60:
                case 0xe0:
                case 0xc0:
                    /* Right of center -> turn to left */
                    handle( -8 );
                    motor( 35 ,40 );
                    break;
            }
            break;

        case 31:
            /* Left crank clearing processing ? wait until stable */
            if( cnt1 > 200 ) {
                pattern = 32;
                cnt1 = 0;
            }
            break;

        case 32:
            /* Left crank clearing processing ? check end of turn */
            if( sensor_inp(MASK3_3) == 0x60 ) {
                led_out( 0x0 );
                pattern = 11;
                cnt1 = 0;
            }
            break;

        case 41:
            /* Right crank clearing processing ? wait until stable */
            if( cnt1 > 200 ) {
                pattern = 42;
                cnt1 = 0;
            }
            break;

        case 42:
            /* Right crank clearing processing ? check end of turn */
            if( sensor_inp(MASK3_3) == 0x06 ) {
                led_out( 0x0 );
                pattern = 11;
                cnt1 = 0;
            }
            break;

        case 51:
            /* Processing at 1st right half line detection */
            led_out( 0x2 );
            handle( 0 );
            motor( 0 ,0 );
            pattern = 52;
            cnt1 = 0;
            break;

        case 52:
            /* Read but ignore 2nd time */
            if( cnt1 > 100 ){
                pattern = 53;
                cnt1 = 0;
            }
            break;

        case 53:
            /* Trace, lane change after right half line detection */
            if( sensor_inp(MASK4_4) == 0x00 ) {
                handle( 15 );
                motor( 40 ,31 );
                pattern = 54;
                cnt1 = 0;
                break;
            }
            switch( sensor_inp(MASK3_3) ) {
                case 0x00:
                    /* Center -> straight */
                    handle( 0 );
                    motor( 40 ,40 );
                    break;
                case 0x04:
                case 0x06:
                case 0x07:
                case 0x03:
                    /* Left of center -> turn to right */
                    handle( 8 );
                    motor( 40 ,35 );
                    break;
                case 0x20:
                case 0x60:
                case 0xe0:
                case 0xc0:
                    /* Right of center -> turn to left */
                    handle( -8 );
                    motor( 35 ,40 );
                    break;
                default:
                    break;
            }
            break;

        case 54:
            /* Right lane change end check */
            if( sensor_inp( MASK4_4 ) == 0x3c ) {
                led_out( 0x0 );
                pattern = 11;
                cnt1 = 0;
            }
            break;

        case 61:
            /* Processing at 1st left half line detection */
            
			( 0x1 );
            handle( 0 );
            motor( 0 ,0 );
            pattern = 62;
            cnt1 = 0;
            break;

        case 62:
            /* Read but ignore 2nd time */
            if( cnt1 > 100 ){
                pattern = 63;
                cnt1 = 0;
            }
            break;

        case 63:
            /* Trace, lane change after left half line detection */
            if( sensor_inp(MASK4_4) == 0x00 ) {
                handle( -15 );
                motor( 31 ,40 );
                pattern = 64;
                cnt1 = 0;
                break;
            }
            switch( sensor_inp(MASK3_3) ) {
                case 0x00:
                    /* Center -> straight */
                    handle( 0 );
                    motor( 40 ,40 );
                    break;
                case 0x04:
                case 0x06:
                case 0x07:
                case 0x03:
                    /* Left of center -> turn to right */
                    handle( 8 );
                    motor( 40 ,35 );
                    break;
                case 0x20:
                case 0x60:
                case 0xe0:
                case 0xc0:
                    /* Right of center -> turn to left */
                    handle( -8 );
                    motor( 35 ,40 );
                    break;
                default:
                    break;
            }
            break;

        case 64:
            /* Left lane change end check */
            if( sensor_inp( MASK4_4 ) == 0x3c ) {
                led_out( 0x0 );
                pattern = 11;
                cnt1 = 0;
            }
            break;

        default:
            /* If neither, return to standby state */
            pattern = 0;
            break;
	}
}