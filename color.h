#ifndef _color_H
#define _color_H

#include <xc.h>
#include "serial.h"
#include "interrupts.h"
#include "dc_motor.h"
#define _XTAL_FREQ 64000000 //note intrinsic _delay function is 62.5ns at 64,000,000Hz  

/********************************************************************
 *  Definition of RGBC structure. Floats are used as norm_RGBC are in decimals. 
 *******************************************************************/
typedef struct RGBC_val { 
	float R;
	float G;
	float B;
    float C;
    float norm_R;
    float norm_G;
    float norm_B;
    float norm_C;

} RGBC_val;

/********************************************************************
 *  Definition of Clear Thresholds for motor response.. 
 *******************************************************************/
float CR1L; 
float CR2U;
float CR2L;
float CR3U;
float CR3L;


char motor_return; //Stores wheter the buggy is in forward or retrace mode
char buggy_path[15]; //Stores the colours the buggy has seen in the maze
int buggy_step; //stores which step the buggy is on
int lost_ctr; //Attempts made to read colour if lost
int black_clear; //Clear value of a black surface
int upperThreshold = 2000; //Upper Threshold for interrupt
int lowerThreshold = 0; //Lower Threshold for interrupt


unsigned int color_read_Red(void);
unsigned int color_read_Green(void);
unsigned int color_read_Blue(void);
unsigned int color_read_Clear(void);

void color_click_init(void);
void color_writetoaddr(char address, char value);
void color_read_RGBC(struct RGBC_val *temp);
void color_normalise(struct RGBC_val *RGBC);
char colorVal2String(char *buf,struct RGBC_val *temp);
void RGBC2Serial(char *str);

void color_click_init(void);

void rLED(void);
void gLED(void);
void bLED(void);
void tricolorLED(void);
void tricolorLEDoff(void);

void RGBC_timing_register(void);

char motor_response(struct RGBC_val *temp, struct DC_motor *L, struct DC_motor *R);
void motor_retrace(char *buggy_path, struct DC_motor *mL, struct DC_motor *mR);
void rangeCalibrate(struct RGBC_val *RGBC, struct DC_motor *mL,struct DC_motor *mR );
void calibSwitchInit(void);
#endif
