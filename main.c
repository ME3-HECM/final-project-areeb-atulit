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
    //Variable/structure declarations and initialisations
    RGBC_val RGBC; //initialise object of struct RGBC_val
    char buf[100]; //buffer to store rbgc values
    motor_return = 0;// variable that tells motor whether its in forward or retrace mode
    buggy_step=0;//count position in buggy motion history path
    lost_ctr=0;
    interrupt_ctr=0;//count number of interrupts executed
    //int pathSet=0; //counter variable to reset path history
    //reset each element of path history to zero
//    for(pathSet=0;pathSet<100;pathSet++)
//    {
//        buggy_path[pathSet]=0;
//    }
    
    //Function calls
    color_click_init();
    Interrupts_init();
    tricolorLED(); //turn on tri-color LED
    //initialise colorclick interrupt
    Color_Interrupts_init();
    Color_Interrupts_threshold(upperThreshold, lowerThreshold);
    persistence_register();
    
    //initialise motor
    initDCmotorsPWM(200);
    DC_motor mL, mR;
    motorLinit(&mL);
    motorRinit(&mR);
    
    //TRIS set for motor control
    motorTRIS(&mL,&mR);
    
    //set tris and lat for various LEDs for debugging purposes
    //inbuilt PIC leds
    TRISDbits.TRISD7 = 0;
    LATDbits.LATD7 = 0;
    TRISHbits.TRISH3 = 0;
    LATHbits.LATH3 = 0;
    
    // buggy leds
    //brake signal
    TRISHbits.TRISH0 = 0;
    LATHbits.LATH0 = 0;
    
    //left signal 
    TRISFbits.TRISF0 = 0;
    LATFbits.LATF0 = 0;
    
     //right signal 
    TRISDbits.TRISD4 = 0;
    LATDbits.LATD4 = 0;
    
    //beam light
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 0;

   while (1) {
       if(buggy_step==0) LATDbits.LATD3 = 1;
       if(buggy_step==1) LATDbits.LATD4 = 1;
       if(buggy_step==2) LATHbits.LATH0 = 1;
       
        fullSpeedAhead(&mL, &mR); // go straight initially
        if (interrupt_flag == 1 && interrupt_ctr>1) {//if pic interrupt is triggered, go into this if statement
            norm_stop(&mL, &mR);  //first stop the buggy
            __delay_ms(1000);

            LATDbits.LATD7 = 0; //turn off the led which monitors interrupt activity
            if(motor_return == 0){    // check if motor is in forward mode
                wallSmash(&mL, &mR);  // press against the wall to allow for more consistent colour readings. 
                __delay_ms(600);     
                color_read_RGBC(&RGBC);// read RGBC values in one go
                color_normalise(&RGBC);// normalise colour values using formula Clear/(colour R/G/B). Normalised clear is clear/(clear value against black wall)
                LATHbits.LATH3 = !LATHbits.LATH3;   //toggle LED to show that a motor response to a colour is occuring
                buggy_path[buggy_step] = motor_response(&RGBC,&mL,&mR);//perform motor activity for colour detected, and add colour to buggy path history array
                buggy_step++;//increment the buggy path step counter
            } else {// if motor is in retrace mode
                motor_retrace(&buggy_path, &mL, &mR);//reverse the corresponding forward step
                buggy_step--;// decrement counter to go to previous step of forward journey
                //the following code brings the buggy to its initial position after its final turn on the return journey is complete. 
                if (buggy_step == 1) {
                    //;fullSpeedAhead(&mL, &mR);
                    //__delay_ms(500);
                    motor_return = 0;
                    norm_stop(&mL, &mR);
                    __delay_ms(500);
                    turnLeft(&mL, &mR);
                    __delay_ms(490);
                    LATHbits.LATH0 = !LATHbits.LATH0;                  
                    norm_stop(&mL, &mR);
                    __delay_ms(2000);
                }
            }    
            LATHbits.LATH3 = 0;//reset led to check which mode the response represents
            interrupt_flag = 0;//reset flag to check for colour
   }
}
}

//----------Code for Serial Term(comment out interrupts.h before using this fucntion)-----------------------------------------------
//void main() {
//    //Variable declarations
//    tricolorLED();
//    TRISHbits.TRISH3 = 0;
//    LATHbits.LATH3 = 0;
//    
//    RGBC_val RGBC; //initialise object of struct RGBC_val
//    char buf[100]; //buffer to store rbgc values
//    initUSART4();
//    color_click_init();
//    
//    while(1) {
//        int test = 0;
//            switch(test){
//                case 0:
//                    tricolorLED();
//
//                    break;
//                case 1:
//                    tricolorLEDoff();
//                    rLED();
//                    break;
//                case 2:
//                    tricolorLEDoff();
//                    gLED();
//                    break;
//                case 3:
//                    tricolorLEDoff();
//                    bLED();
//                    gLED();
//                    break;
//            }
//            
//
//        color_read_RGBC(&RGBC);
//        color_normalise(&RGBC);
//        LATHbits.LATH3 = !LATHbits.LATH3;
//        __delay_ms(1000);
//        colorVal2String(buf, &RGBC);
//        RGBC2Serial(buf);
//        
//    }
//}
// 
////------------------------------------------------------------------------------
