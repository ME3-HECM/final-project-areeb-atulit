#ifndef _DC_MOTOR_H
#define _DC_MOTOR_H

#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include <stdio.h>
#include "interrupts.h"
#include "dc_motor.h"

#define _XTAL_FREQ 64000000

typedef struct DC_motor { //definition of DC_motor structure
    char power;         //motor power, out of 100
    char direction;     //motor direction, forward(1), reverse(0)
    char brakemode;		// short or fast decay (brake or coast)
    unsigned int PWMperiod; //base period of PWM cycle
    unsigned char *posDutyHighByte; //PWM duty address for motor +ve side
    unsigned char *negDutyHighByte; //PWM duty address for motor -ve side
} DC_motor;

//function prototypes
void initDCmotorsPWM(unsigned int PWMperiod); // function to setup PWM
void setMotorPWM(DC_motor *m);
void stop(DC_motor *mL, DC_motor *mR);
void turnLeft(DC_motor *mL, DC_motor *mR);
void turnRight(DC_motor *mL, DC_motor *mR);
void uturn(DC_motor *mL, DC_motor *mR);

void fullSpeedAhead(DC_motor *mL, DC_motor *mR);
void wallSmash(DC_motor *mL, DC_motor *mR);
void reverse(DC_motor *mL, DC_motor *mR);
void motorLinit(DC_motor *mL);
void motorRinit(DC_motor *mR);
void norm_stop(DC_motor *mL, DC_motor *mR);
void turnPrep(DC_motor *mL, DC_motor *mR);
void motorTRIS(DC_motor *mL, DC_motor *mR);
void slowSearch(DC_motor *mL, DC_motor *mR);
#endif
