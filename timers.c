#include <xc.h>
#include "timers.h"

/************************************
 * Function to set up timer 0
************************************/
void Timer0_init(void)
{
    T0CON1bits.T0CS=0b010; // Fosc/4
    T0CON1bits.T0ASYNC=1; // see datasheet errata - needed to ensure correct operation when Fosc/4 used as clock source
    T0CON1bits.T0CKPS=0b1000; // 1:256
    T0CON0bits.T016BIT=1;	//8bit mode	
	
    // it's a good idea to initialise the timer registers so we know we are at 0
    TMR0H=0b00001011;            //write High reg first, update happens when low reg is written to
    TMR0L=0b11011011;
    T0CON0bits.T0EN=1;	//start the timer
}

/************************************
 * Function to return the full 16bit timer value
 * Note TMR0L and TMR0H must be read in the correct order, or TMR0H will not contain the correct value
************************************/
unsigned int get16bitTMR0val(void)
{
	unsigned int a=TMR0L;
    unsigned int b=TMR0H;
    
    return b;
}
