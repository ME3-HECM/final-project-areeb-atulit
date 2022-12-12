#ifndef _color_H
#define _color_H

#include <xc.h>
#include "serial.h"
#include "interrupts.h"
#include "dc_motor.h"
#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

//definition of RGB structure
typedef struct RGBC_val { 
	float R;
	float G;
	float B;
    float C;
    float norm_R;
    float norm_G;
    float norm_B;
    float norm_C;
//    unsigned int R;
//    unsigned int G;
//    unsigned int B;
//    unsigned int C;
//    unsigned int norm_R;
//    unsigned int norm_G;
//    unsigned int norm_B;
//    unsigned int norm_C;
} RGBC_val;


char motor_return;
char buggy_path[15];
int ctr; //stores which step the buggy is on
int amb_red;
int amb_green;
int amb_blue;
int amb_clear;
int upperThreshold = 3000;
int lowerThreshold = 0;
/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
void color_click_init(void);

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value);

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/

unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);
unsigned int color_read_Clear(void);
void color_read_RGBC(struct RGBC_val *temp);
void color_normalise(struct RGBC_val *RGBC);
void color_click_init(void);
char colorVal2String(char *buf,struct RGBC_val *temp);
void tricolorLED(void);
void RGBC2Serial(char *str);
void RGBC_timing_register(void);
char motor_response(struct RGBC_val *temp, struct DC_motor *L, struct DC_motor *R);
void motor_retrace(char *buggy_path, struct DC_motor *mL, struct DC_motor *mR);
#endif
