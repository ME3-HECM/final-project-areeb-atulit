#include <xc.h>
#include "dc_motor.h"

/************************************************************************
 *  Function initialise T2 and CCP for DC motor control
 ***********************************************************************/
void initDCmotorsPWM(unsigned int PWMperiod){
    //initialise your TRIS and LAT registers for PWM  
    
    //configure PPS to map CCP modules to pins
    RE2PPS=0x05; //CCP1 on RE2
    RE4PPS=0x06; //CCP2 on RE4
    RC7PPS=0x07; //CCP3 on RC7
    RG6PPS=0x08; //CCP4 on RG6

    // timer 2 config
    T2CONbits.CKPS=0b0011; // 1:8 prescaler
    T2HLTbits.MODE=0b00000; // Free Running Mode, software gate only
    T2CLKCONbits.CS=0b0001; // Fosc/4

    // Tpwm*(Fosc/4)/prescaler - 1 = PTPER
    // 0.0001s*16MHz/16 -1 = 99
    T2PR=200; //Period reg 10kHz base period
    T2CONbits.ON=1;
    
    //setup CCP modules to output PMW signals
    //initial duty cycles 
    CCPR1H=0; 
    CCPR2H=0; 
    CCPR3H=0; 
    CCPR4H=0; 
    
    //use tmr2 for all CCP modules used
    CCPTMRS0bits.C1TSEL=0;
    CCPTMRS0bits.C2TSEL=0;
    CCPTMRS0bits.C3TSEL=0;
    CCPTMRS0bits.C4TSEL=0;
    
    //configure each CCP
    CCP1CONbits.FMT=1; // left aligned duty cycle (we can just use high byte)
    CCP1CONbits.CCP1MODE=0b1100; //PWM mode  
    CCP1CONbits.EN=1; //turn on
    
    CCP2CONbits.FMT=1; // left aligned
    CCP2CONbits.CCP2MODE=0b1100; //PWM mode  
    CCP2CONbits.EN=1; //turn on
    
    CCP3CONbits.FMT=1; // left aligned
    CCP3CONbits.CCP3MODE=0b1100; //PWM mode  
    CCP3CONbits.EN=1; //turn on
    
    CCP4CONbits.FMT=1; // left aligned
    CCP4CONbits.CCP4MODE=0b1100; //PWM mode  
    CCP4CONbits.EN=1; //turn on
}

/************************************************************************
 *  Function to set CCP PWM output from the values in the motor structure
 ***********************************************************************/
void setMotorPWM(DC_motor *m)
{
    unsigned char posDuty, negDuty; //duty cycle values for different sides of the motor
    
    if(m->brakemode) {
        posDuty=m->PWMperiod - ((unsigned int)(m->power)*(m->PWMperiod))/100; //inverted PWM duty
        negDuty=m->PWMperiod; //other side of motor is high all the time
    }
    else {
        posDuty=((unsigned int)(m->power)*(m->PWMperiod))/100; // PWM duty
        negDuty=0; //other side of motor is low all the time
    }
    
    if (m->direction) {
        *(m->posDutyHighByte)=posDuty;  //assign values to the CCP duty cycle registers
        *(m->negDutyHighByte)=negDuty;       
    } else {
        *(m->posDutyHighByte)=negDuty;  //do it the other way around to change direction
        *(m->negDutyHighByte)=posDuty;
    }
}

/**********************************************
 *  Function to initialise the left motor
 ***********************************************/
void motorLinit(DC_motor *mL)
{
    mL->power = 50; //zero power to start
    mL->direction = 1; //set default motor direction
    mL->brakemode = 1; // brake mode (slow decay)
    mL->posDutyHighByte = (unsigned char *) (&CCPR1H); //store address of CCP1 duty high byte
    mL->negDutyHighByte = (unsigned char *) (&CCPR2H); //store address of CCP2 duty high byte
    mL->PWMperiod = 200;
}

/**********************************************
 *  Function to initialise the right motor
 ***********************************************/
void motorRinit(DC_motor *mR)
{
    mR->power = 50; //zero power to start
    mR->direction = 1; //set default motor direction
    mR->brakemode = 1; // brake mode (slow decay)
    mR->posDutyHighByte = (unsigned char *) (&CCPR3H); //store address of CCP1 duty high byte
    mR->negDutyHighByte = (unsigned char *) (&CCPR4H); //store address of CCP2 duty high byte
    mR->PWMperiod = 200;
}

/**********************************************
 *  Function to decelerate the buggy
 ***********************************************/
void decelerate(DC_motor *mL, DC_motor *mR)
{
    while (mL->power >= 1 && mR->power >= 1)
    {
        mL->power--;
        mR->power--;
        __delay_ms(10);
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
    mL->power = 0;
    mR->power = 0;
    setMotorPWM(mL);
    setMotorPWM(mR);
    
}

/**********************************************
 *  Function to stop the buggy
 ***********************************************/
void norm_stop(DC_motor *mL, DC_motor *mR)
{
    mL->power = 0;
    mR->power = 0;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

/**********************************************
 *  Function to make the buggy turn left
 ***********************************************/
void turnLeft(DC_motor *mL, DC_motor *mR)
{
    mL->power = 60;
    mR->power = 60;
    mL->brakemode = 1;
    mR->brakemode = 1;
    mL->direction = 0;
    mR->direction = 1;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

/**********************************************
 *  Function to make the buggy turn right
 ***********************************************/
void turnRight(DC_motor *mL, DC_motor *mR)
{
    mL->power = 60;
    mR->power = 60;
    mL->brakemode = 1;
    mR->brakemode = 1;
    mR->direction = 0;
    mL->direction = 1;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

/**********************************************
 *  Function to slowly move 
 ***********************************************/
void slowSearch(DC_motor *mL, DC_motor *mR)
{
    mL->power = 40;
    mR->power = 40;
    mL->brakemode = 1;
    mR->brakemode = 1;
    mR->direction = 0;
    mL->direction = 1;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

/**********************************************
 *  Function to perform a U-Turn
 ***********************************************/
void uturn(DC_motor *mL, DC_motor *mR){
    mL->power = 45;
    mR->power = 45;
    mL->brakemode = 1;
    mR->brakemode = 1;
    mR->direction = 0;
    mL->direction = 1;
    setMotorPWM(mL);
    setMotorPWM(mR);
}

/**********************************************
 *  Function to move forward
 ***********************************************/
void fullSpeedAhead(DC_motor *mL, DC_motor *mR)
{
    mL->power =5;
    mR->power = 5;
    mR->direction = 1;
    mL->direction = 1;
    mL->brakemode = 1;
    mR->brakemode = 1;
    while (mL->power <= 50 && mR->power <=50)
    {
        mL->power++;
        mR->power++;
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
    mL->power = 50;
    mR->power = 50;
    setMotorPWM(mL);
    setMotorPWM(mR);
    
}

/**********************************************
 *  Function to accelerate into wall
 ***********************************************/
void wallSmash(DC_motor *mL, DC_motor *mR)
{
    mL->power =50;
    mR->power = 50;
    mR->direction = 1;
    mL->direction = 1;
    mL->brakemode = 1;
    mR->brakemode = 1;
    while (mL->power <= 85 && mR->power <=85)
    {
        mL->power++;
        mR->power++;
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
    mL->power = 85;
    mR->power = 85;
    setMotorPWM(mL);
    setMotorPWM(mR);
    
}

/**********************************************
 *  Function to reverse
 ***********************************************/
void reverse(DC_motor *mL, DC_motor *mR)
{
    mL->power = 5;
    mR->power = 5;
    mR->direction = 0;
    mL->direction = 0;
    mL->brakemode = 1;
    mR->brakemode = 1;
    while (mL->power <= 20 && mR->power <=20)
    {
        mL->power++;
        mR->power++;
        __delay_ms(10);
        setMotorPWM(mL);
        setMotorPWM(mR);
    }
    mL->power = 20;
    mR->power = 20;
    setMotorPWM(mL);
    setMotorPWM(mR);
    
}

/**********************************************
 *  Function to prepare for turn
 ***********************************************/
void turnPrep(DC_motor *mL, DC_motor *mR){
        reverse(mL, mR);
        __delay_ms(600);
        norm_stop(mL, mR);
        __delay_ms(300);
}

/**********************************************
 *  Function to read one set of RGBC values
 ***********************************************/
void motorTRIS(DC_motor *mL, DC_motor *mR){
    TRISEbits.TRISE2 = 0;
    TRISEbits.TRISE4 = 0;
    TRISCbits.TRISC7 = 0;
    TRISGbits.TRISG6 = 0;
}