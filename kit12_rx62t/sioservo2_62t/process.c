#include "leds.h"
#include "motors.h"
#include "sensors.h"
#include "switches.h"

extern unsigned long cnt0;
extern unsigned long cnt1;
extern int pattern;


extern volatile int accele_l;
extern volatile int accele_r;

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

void process_1() { // ours
	switch( pattern ) {
		/****************************************************************
        Pattern-related
         00: wait for switch input
		 01: check if start bar is open
         10: drive
		 20 - 29 : 90 degree turn
		 30 - 39 : changing track right
		 40 - 49 : changing track left
        ****************************************************************/
		case 00:
			/* Wait for switch input */
            if( pushsw_get() ) {
                //pattern = 01;
				pattern = 10; // straight to 10 for now, no startbar checking
				led_out( 0x0 );
                cnt0 = 0;
                break;
            }
            if( cnt0 < 100 ) {          /* LED flashing processing     */
                led_out( 0x1 );
            } else if( cnt0 < 200 ) {
                led_out( 0x2 );
            } else {
                cnt0 = 0;
            }
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
		case 10:
			
			// all two diodies blink
			led_out( 0x3 );
			
			/* Drive */
			if( check_crossline() ) {   // Cross line check            
                pattern = 20;
                break;
            }
            if( check_rightline() ) {   // Right half line detection check
                pattern = 30;
                break;
            }
            if( check_leftline() ) {    // Left half line detection check
                pattern = 40;
                break;
            }
			if ( check_noline() ) {
				// stop
				accele_l = 0;
				accele_r = 0;
				break;
			}
				
		   break;
		  
		case 20:
			// ne znamo jos, skretanje po 90 stepeni (levo, desno? brojimo...)
			
			if ( check_noline() ) {
				// stop
				accele_l = 0;
				accele_r = 0;
				break;
			}
			
			break;
			
		case 30:
			// videli smo jedan RIGHTLINE
			if (sensor_inp(MASK3_3) == 0x00)
				pattern = 31;
			break;
			
		case 31:
			// cekamo drugi RIGHTLINE
			if (check_rightline())
				pattern = 32;
			break;
		
		case 32:
			// videli smo i drugi RIGHTLINE
			if (sensor_inp(MASK3_3) == 0x00)
				pattern = 33;
			break;
		
		case 33:
		
			// diode 1 blinks
			led_out( 0x1 );
			
			// CHANGE LANE, RIGHT
			if (sensor_inp(MASK4_4) == 0x00) { // all black
				pattern = 34;
			}
			break;
			
		case 34:
			// wait for the line, then turn left
			if (sensor_inp(0x18) == 0x18) { // wait for the 2 central LEDs
				pattern = 35;
			}
			break;
			
		case 35:
			if (sensor_inp(MASK3_3) == 0x00) { // wait for the formula to catch the line properly, then go back to pattern 10
				pattern = 10;
			}
			
		
		case 40:
			// videli smo jedan LEFTLINE
			if (sensor_inp(MASK3_3) == 0x00)
				pattern = 41;
			break;
			
		case 41:
			// cekamo drugi LEFTLINE
			if (check_leftline())
				pattern = 42;
			break;
		
		case 42:
			// videli smo i drugi LEFTLINE
			if (sensor_inp(MASK3_3) == 0x00)
				pattern = 43;
			break;
		
		case 43:
			// diode 2 blinks
			led_out( 0x2 );
			
			// CHANGE LANE, LEFT
			if (sensor_inp(MASK4_4) == 0x00) { // all black
				pattern = 44;
			}
			break;
			
		case 44:
			// wait for the line, then turn right
			if (sensor_inp(0x18) == 0x18) { // wait for the 2 central LEDs
				pattern = 45;
			}
			break;
			
		case 45:
			if (sensor_inp(MASK3_3) == 0x00) { // wait for the formula to catch the line properly, then go back to pattern 10
				pattern = 10;
			}
		
		default:
		   break;
	}
}
