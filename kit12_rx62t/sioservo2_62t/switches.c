#include "switches.h"

#include "iodefine.h"

/***********************************************************************/
/* DIP switch value read                                               */
/* Return values: Switch value, 0 to 15                                */
/***********************************************************************/
unsigned char dipsw_get( void )
{
    unsigned char sw,d0,d1,d2,d3;

    d0 = ( PORT6.PORT.BIT.B3 & 0x01 );  /* P63~P60 read                */
    d1 = ( PORT6.PORT.BIT.B2 & 0x01 ) << 1;
    d2 = ( PORT6.PORT.BIT.B1 & 0x01 ) << 2;
    d3 = ( PORT6.PORT.BIT.B0 & 0x01 ) << 3;
    sw = d0 | d1 | d2 | d3;

    return  sw;
}

/***********************************************************************/
/* Push-button in MCU board value read                                 */
/* Return values: Switch value, ON: 1, OFF: 0                          */
/***********************************************************************/
unsigned char buttonsw_get( void )
{
    unsigned char sw;

    sw = ~PORTE.PORT.BIT.B5 & 0x01;     /* Read ports with switches    */

    return  sw;
}

/***********************************************************************/
/* Push-button in motor drive board value read                         */
/* Return values: Switch value, ON: 1, OFF: 0                          */
/***********************************************************************/
unsigned char pushsw_get( void )
{
    unsigned char sw;

    sw  = ~PORT7.PORT.BIT.B0 & 0x01;    /* Read ports with switches    */

    return  sw;
}


