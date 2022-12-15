#include <xc.h>
#include "interrupts.h"
#include "dc_motor.h"
#include "color.h"
/************************************
 * Function to turn on interrupts and set if priority is used
 * Note you also need to enable peripheral interrupts in the INTCON register to use CM1IE.
************************************/

void Interrupts_init(void)
{
	// turn on global interrupts, peripheral interrupts and the interrupt source 
	// It's a good idea to turn on global interrupts last, once all other interrupt configuration is done.
    
    TRISBbits.TRISB0=1;
    ANSELBbits.ANSELB0=0; //Turns off analogue input
    PIR0bits.INT0IF = 0;
    PIE0bits.INT0IE = 1;
    IPR0bits.INT0IP = 1;
    INTCONbits.INT0EDG=0; //Trigger on falling edge
    INTCONbits.IPEN=1;//Enable priority 
    INTCONbits.PEIE=1;
    INTCONbits.GIE=1; //Turn on interrupts globally (when this is off, all interrupts are deactivated)
    
}

void Color_Interrupts_init(void)
{
    
   color_writetoaddr(0x00, 0b00010011);
   __delay_ms(10) ;
   
}

void Color_Interrupts_threshold(unsigned int upperThreshold, unsigned int lowerThreshold)
{
	color_writetoaddr(0x04, lowerThreshold);
    color_writetoaddr(0x05, lowerThreshold>>8);
    color_writetoaddr(0x06, upperThreshold);
    color_writetoaddr(0x07, upperThreshold>>8);
}

void persistence_register(void)
{
	color_writetoaddr(0x0C, 0b010);
}

void Color_Interrupts_clear(void)
{
    I2C_2_Master_Start();            //Start condition   
    I2C_2_Master_Write(0x52 | 0x00); //7 bit device address + Write mode
    I2C_2_Master_Write(0b11100110);    
    I2C_2_Master_Stop();             //Stop condition
    Color_Interrupts_init();
    persistence_register();
    Color_Interrupts_threshold(upperThreshold, lowerThreshold);
    
}

/************************************
 * High priority interrupt service routine
 * Make sure all enabled interrupts are checked and flags cleared
************************************/
void __interrupt(high_priority) HighISR()
{
    if (PIR0bits.INT0IF == 1) { //check the interrupt source
        interrupt_flag = 1; 
        interrupt_ctr++;
        Color_Interrupts_clear(); //Clear the colour clicker flag
        PIR0bits.INT0IF = 0; //clear the interrupt flag!
    }  
}


