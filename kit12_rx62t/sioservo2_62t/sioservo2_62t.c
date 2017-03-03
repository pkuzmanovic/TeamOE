#include "init.h"
#include "common.h"
#include "leds.h"
#include "motors.h"
#include "sensors.h"
#include "switches.h"

#include "iodefine.h"

// filtering buffer length
#define BUFF_LENGTH 5

volatile unsigned long cnt0 = 0;
volatile unsigned long cnt1 = 0;
volatile unsigned long cnt2 = 0;
volatile unsigned long cnt3 = 0;

// keep track of the last 9 sensor configurations
volatile int sensor_buff[BUFF_LENGTH] = {{0}};
// sums of the last 9 sensor configurations, by bits
volatile int sensor_sums[8] = {{0}};
// slow-changing sensor configuration
volatile int sensor_filtered = 0x18;
volatile int sensor_last = 0x00;
// buffer counter
volatile int sensor_head = 0;

volatile int angle = 0;
//volatile int lvl = 20;

volatile int accele_l = 0;
volatile int accele_r = 0;

int pattern = 0;

void init(void);

void process_0(); // default
void process_1(); // ours

/***********************************************************************/
/* Main program                                                        */
/***********************************************************************/
void main(void)
{
   unsigned char   now_sw;             /* memorize dip switch now      */
   unsigned char   before_sw;          /* memorize dip switch last time*/
	
	/* Initialization */ 
   init();
   
   /* variable initialization */
    before_sw = dipsw_get();
	
   /* Main loop */
   while (1) {
	   /* read dip switch */
   		now_sw = dipsw_get();

    	/* comparing with switch at last time */
    	if( before_sw != now_sw ) {
        	/* mismatch Å®Å@update value at last time, clear timer value */
        	before_sw = now_sw;
        	cnt1 = 0;
    	}
		
		/* choose operation check mode by value of dip switch */
    	switch( now_sw ) {
			case 0:
				process_0();
				break;
			case 1:
				process_1();
				break;
			default:
				process_1();
				break;
		}
   }
}

/* Initialization */
void init(){
	/* Initialize MCU functions */
	init_hw();
	
	/* Initialize global values */
	cnt0 = 0;
	pattern = 0;
	
	/* Initialize micom car state */
    handle( 0 );
    motor( 0, 0 );
	
}

/***********************************************************************/
/* Interrupt                                                           */
/***********************************************************************/

// prekid koji se koristi za ocitavanje senzora i postavljanje zeljeneog ugla za skretanje na osnovu njih
#pragma interrupt Excep_CMT0_CMI0(vect=28)
void Excep_CMT0_CMI0(void)
{
	int i, sensor_cur;
	
    cnt0++;
	cnt1++;
	
	// FILTRIRANJE //
	// trenutna vrednost
	sensor_cur = sensor_inp(MASK4_4);
	// pamtimo
	sensor_buff[sensor_head] = sensor_cur;
	// poslednje ubaceni bafer
	sensor_last = (sensor_head!=BUFF_LENGTH-1) ? sensor_buff[sensor_head+1] : sensor_buff[0];
	// filtriranje
	sensor_filtered = 0;
	for (i = 0; i < 8; i++) {
		sensor_sums[i] -= (( sensor_last & (1 << i)) >> i);  // izbacujemo poslednji
		sensor_sums[i] += (( sensor_cur & (1 << i)) >> i);   // ubacujemo trenutni
		if (sensor_sums[i] > (double)(BUFF_LENGTH/2)) {
			sensor_filtered += (1 << i);
		}
	}
	sensor_head = (sensor_head + 1) % BUFF_LENGTH;
}


// prekid koji se koristi za regulaciju skretanja servoa i DC motora
#pragma interrupt Excep_CMT1_CMI1(vect=29)
void Excep_CMT1_CMI1(void)
{	int cur_angle = 0;
	double ang = 0;
	int cur_accl, cur_accr, accl, accr;
	
	// privremeno da se ne krece dok se ne klikne dugme
	if(pattern < 10)
		return;
		
	// regulacija servoa, racuna se razlika i deli sa 4 ima PI regulaciju
	// na svaki 100 poziv rutine (mozda i malo brze, 50?)
	cnt2++;
	if (cnt2 == 100){
		cur_angle = (SERVO_CENTER - MTU3.TGRD)/ HANDLE_STEP;
		if(cur_angle < 45 || cur_angle > -45)
		{
			ang = (angle - cur_angle)/4;
			MTU3.TGRD -= (int) ang* HANDLE_STEP;
		}
		cnt2 = 0;	
	}
	
	
	// regulacija DC motora, ovo treba cesce da se izvrsava
	// racuna se razlika i deli sa 3 ima PI regulaciju
	// na svaki 21. put
	cnt3++;
	if (cnt3 == 21) {
		cur_accl = 100 * MTU4.TGRC/( PWM_CYCLE - 1 );
		cur_accr = 100 * MTU4.TGRD/( PWM_CYCLE - 1 );
		
		accr = cur_accr+(accele_r - cur_accr)/3;
		accl = cur_accl+(accele_l - cur_accl)/3;
		
		// proverava granice da ne pretera
		if(accr > 100)
			accr = 100;
		else if(accr < -100)
			accr = -100;	
		if(accl > 100)
			accl = 100;
		else if(accl < -100)
			accl = -100;
			
		motor(accl,accr); 
		cnt3 = 0;
	}
}
