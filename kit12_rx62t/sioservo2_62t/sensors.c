#include "sensors.h"

#include "iodefine.h"

extern volatile int sensor_filtered;

/***********************************************************************/
/* Sensor state detection                                              */
/* Arguments:       masked values                                      */
/* Return values:   sensor value                                       */
/***********************************************************************/
unsigned char sensor_inp( unsigned char mask )
{
    unsigned char sensor;

    sensor  = ~PORT4.PORT.BYTE;
	
	sensor = (USE_INVERTED_LOGIC) ? (~sensor) : (sensor);

    sensor &= mask;

    return sensor;
}

/***********************************************************************/
/* Read start bar detection sensor                                     */
/* Return values: Sensor value, ON (bar present):1,                    */
/*                              OFF (no bar present):0                 */
/***********************************************************************/
unsigned char startbar_get( void )
{
    unsigned char b;

    b  = ~PORT4.PORT.BIT.B0 & 0x01;     /* Read start bar signal       */

    return  b;
}

/***********************************************************************/
/* Cross line detection processing                                     */
/* Return values: 0: no cross line, 1: cross line                      */
/***********************************************************************/
int check_crossline( void )
{
    unsigned char b;
    int ret = 0;

    ret = 0;
    b = sensor_filtered;
    if( b==CROSSLINE ) {
        ret = 1;
    }
    return ret;
}

/***********************************************************************/
/* Right half line detection processing                                */
/* Return values: 0: not detected, 1: detected                         */
/***********************************************************************/
int check_rightline( void )
{
    unsigned char b;
    int ret = 0;

    ret = 0;
  	b = sensor_filtered;
    if(  b==RIGHTLINE1 || b==RIGHTLINE2 ) {
        ret = 1;
    }
    return ret;
}

/***********************************************************************/
/* Left half line detection processing                                 */
/* Return values: 0: not detected, 1: detected                         */
/***********************************************************************/
int check_leftline( void )
{
    unsigned char b;
    int ret = 0;

    //ret = 0;
    b = sensor_filtered;
    if( b==LEFTLINE1 || b==LEFTLINE2 ) {
        ret = 1;
    }
    return ret;
}

/**********************************************************************************/
/* No line present (all black for noniverted logic, all white for inverted logic) */
/* Return values: 0: not detected, 1: detected                                    */
/**********************************************************************************/
int check_noline( void )
{
    unsigned char b;
    int ret = 0;

	b = sensor_filtered;
    if( b==NOLINE ) {
        ret = 1;
    }
    return ret;
}

int check_straightline( void ) {
	
	unsigned char b;
    int ret = 0;

	b = sensor_filtered;
    if( b==STRAIGHTLINE ) {
        ret = 1;
    }
    return ret;
	
}

