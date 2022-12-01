// CONFIG1L
#pragma config FEXTOSC = HS     // External Oscillator mode Selection bits (HS (crystal oscillator) above 8 MHz; PFM set to high power)
#pragma config RSTOSC = EXTOSC_4PLL// Power-up default value for COSC bits (EXTOSC with 4x PLL, with EXTOSC operating per FEXTOSC bits)

// CONFIG3L
#pragma config WDTCPS = WDTCPS_31// WDT Period Select bits (Divider ratio 1:65536; software control of WDTPS)
#pragma config WDTE = OFF        // WDT operating mode (WDT enabled regardless of sleep)

#include <xc.h>
#include <pic18f67k40.h>
#include "dc_motor.h"
#include "i2c.h"
#include "color.h"
#include "serial.h"
#include "interrupts.h"
#include "timers.h"

#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

void main(){
    //Variable declarations
    struct RGBC_val RGBC;
    //initialisations
    color_click_init();
    initUSART4();
    //initDCmotorsPWM(20);
    //to turn on the additional buggy leds, find corresponding clickerboard pin and turn it on
    TRISDbits.TRISD3 = 0;
    LATDbits.LATD3 = 1;
    
    tricolorLED();
    char rgb_address[]={0x16, 0x18, 0x1A}; //stores red green and blue register addresses 
//    volatile unsigned int red_val=0;
//    volatile unsigned int blue_val=0;
//    volatile unsigned int green_val=0;

//        red_val=color_read(rgb_address[0]);
//        __delay_ms(500);
//        blue_val=color_read(rgb_address[1]);
//        __delay_ms(500);
//        green_val=color_read(rgb_address[2]);
//        __delay_ms(500); 
        //colorVal2String(&red,300);
    char buf[100];
    color_read_RGBC(&RGBC);
    colorVal2String(buf,&RGBC);
    RGBC2Serial(colorVal2String(buf,&RGBC));     
}