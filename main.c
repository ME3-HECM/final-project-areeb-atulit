// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <stdio.h>
#include <pic18f67k40.h>
#include "dc_motor.h"
#include "i2c.h"
#include "color.h"
#include "serial.h"
#include "interrupts.h"
#include "timers.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz 


void main() {
    //Variable declarations
    tricolorLED();
    RGBC_val RGBC; //initialise object of struct RGBC_val
    char buf[100]; //buffer to store rbgc values
    //initialisations
    color_click_init();
       
    Interrupts_init();
    Color_Interrupts_init();
    Color_Interrupts_threshold(upperThreshold, lowerThreshold);
    persistence_register();
    
    initDCmotorsPWM(200);
    DC_motor mL, mR;
    motorLinit(&mL);
    motorRinit(&mR);

    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
    int amb_red = color_read_Red();
    int amb_green = color_read_Blue();
    int amb_blue = color_read_Green();
    int amb_clear = color_read_Clear();
    motor_return = 0;

   while (1) {
        
        fullSpeedAhead(&mL, &mR);
        //color_read_RGBC(&RGBC);
        if (interrupt_flag == 1 ) {
            //Color_Interrupts_clear();
            norm_stop(&mL, &mR);
            __delay_ms(1000);
            LATDbits.LATD7 = 0;
            color_read_RGBC(&RGBC);
            color_normalise(&RGBC);
//            buggy_path[ctr] = motor_response(&RGBC,&mL,&mR);
//            ctr++;
            if(motor_return == 0){
                LATHbits.LATH3 = !LATHbits.LATH3;
                buggy_path[ctr] = motor_response(&RGBC,&mL,&mR);
                ctr++;
            }
            else{
                motor_retrace(&buggy_path, &mL, &mR); 
            }
   
            LATHbits.LATH3 = 0;
            interrupt_flag = 0;
//            if (RGBC.R >5000){
//                reverse(&mL, &mR);
//                __delay_ms(50);
//                turnRight(&mL, &mR); //turn right if red identified
//                __delay_ms(210);
//                norm_stop(&mL, &mR);
//                __delay_ms(100);
//            }
//        }


   }
}
}

//----------Code for Serial Term(comment out interrupts.h before using this fucntion)-----------------------------------------------
/*void main() {
    //Variable declarations
    tricolorLED();
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
    RGBC_val RGBC; //initialise object of struct RGBC_val
    char buf[100]; //buffer to store rbgc values
    initUSART4();
    color_click_init();
    
    while(1) {
        color_read_RGBC(&RGBC);
        color_normalise(&RGBC);
        LATHbits.LATH3 = !LATHbits.LATH3;
        __delay_ms(500);
        colorVal2String(buf, &RGBC);
        RGBC2Serial(buf);
    }
}
 */
//------------------------------------------------------------------------------
