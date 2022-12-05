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

void main() {
    //Variable declarations
    struct RGBC_val RGBC; //initialise object of struct RGBC_val
    char buf[100]; //buffer to store rbgc values
    int upperThreshold = 2500;
    int lowerThreshold = 0;
    //initialisations
    color_click_init();
    Color_Interrupts_init();
    Color_Interrupts_threshold(upperThreshold, lowerThreshold);
    persistence_register();
    initUSART4();
    initDCmotorsPWM(200);
    DC_motor mL, mR;
    motorLinit(&mL);
    motorRinit(&mR);

    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
    //    DC_motor mL, mR; //declare two DC_motor structures
    //    motorLinit(&mL);
    //    motorRinit(&mR);

    tricolorLED();
    while (1) {
        color_read_RGBC(&RGBC);
        colorVal2String(buf, &RGBC);
//        RGBC2Serial(colorVal2String(buf, &RGBC));
       // fullSpeedAhead(&mL, &mR);
        __delay_ms(2000);
        if (interrupt_flag == 0) {
            norm_stop(&mL, &mR);
            __delay_ms(2000);
            interrupt_flag = 1;
            turnRight(&mL, &mR);
            __delay_ms(500);
            Color_Interrupts_clear();
        }
        norm_stop(&mL, &mR);
        __delay_ms(500);
    }
}