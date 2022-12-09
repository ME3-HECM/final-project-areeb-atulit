#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include <stdio.h>
#include "interrupts.h"
#include "dc_motor.h"

void color_click_init(void)
{   
    //setup colour sensor via i2c interface
    I2C_2_Master_Init();      //Initialise i2c Master

     //set device PON
	 color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up
    
    //turn on device ADC
	color_writetoaddr(0x00, 0x03);

    //set integration time
	color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value){
    I2C_2_Master_Start();         //Start condition
    I2C_2_Master_Write(0x52 | 0x00);     //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address);    //command + register address
    I2C_2_Master_Write(value);    
    I2C_2_Master_Stop();          //Stop condition
}

unsigned int color_read_Red(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x16);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Green(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x18);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Blue(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x1A);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}

unsigned int color_read_Clear(void)
{
	unsigned int tmp;
	I2C_2_Master_Start();         //Start condition
	I2C_2_Master_Write(0x52 | 0x00);     //7 bit address + Write mode
	I2C_2_Master_Write(0xA0 | 0x14);    //command (auto-increment protocol transaction) + start at RED low register
	I2C_2_Master_RepStart();			// start a repeated transmission
	I2C_2_Master_Write(0x52 | 0x01);     //7 bit address + Read (1) mode
	tmp=I2C_2_Master_Read(1);			//read the Red LSB
	tmp=tmp | (I2C_2_Master_Read(0)<<8); //read the Red MSB (don't acknowledge as this is the last read)
	I2C_2_Master_Stop();          //Stop condition
	return tmp;
}
void RGBC2Serial(char *str){
    __delay_ms(200);
    sendStringSerial4(str);
}
void color_read_RGBC(struct RGBC_val *temp){
    temp->R = color_read_Red();
    temp->B = color_read_Blue();
    temp->G = color_read_Green();   
    temp->C = color_read_Clear();
}
char colorVal2String(char *buf,struct RGBC_val *temp) {
    sprintf(buf,"RGBC:%i %i %i %i\n",temp->R, temp->G, temp->B, temp->C); 
    return buf;
}


void tricolorLED(void){
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 1;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4= 1;
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 1;
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1;
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 1;
}

 void motor_response(struct RGBC_val *temp, struct DC_motor *mL, struct DC_motor *mR) {
    if (temp->R > 5000) { //Red Colour (Turn 90deg Right)
     for (int j = 0; j <= 15; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnRight(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    else if (temp->R > 3000 && temp->G > 3000) { //Green Colour (Turn 90deg Left)
    for (int j = 0; j <= 15; j++) { //for 135 deg, j=20, for 90 deg, j=15
        turnLeft(mL, mR);
        __delay_ms(30);
        norm_stop(mL, mR);
        __delay_ms(30);
    }
    }
    else if (temp->R >1000) { //Dark Blue (Turn left 180)
     for (int j = 0; j <= 29; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnLeft(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    
    else if (temp->R >12000 && temp->G > 7000) { //Yellow(Reverse 1 square and turn right 90)
     reverse(mL, mR);
     __delay_ms(10);
     norm_stop(mL, mR);
     __delay_ms(100);
     for (int j = 0; j <= 16; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnRight(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    else if (temp->R >10000 && temp->G > 7000 && temp->C > 20000&& temp->C < 22000) { //Pink(Reverse 1 square and turn left 90)
     reverse(mL, mR);
     __delay_ms(10);
     norm_stop(mL, mR);
     __delay_ms(300);
     for (int j = 0; j <= 12; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnLeft(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    else if (temp->R >8500 && temp->R < 9500 && temp->G > 7000 && temp->C > 13000 && temp->C < 15000) { //Orange(Turn Right 135)
     for (int j = 0; j <= 23; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnRight(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    else if (temp->R >3500&& temp->R < 4500 && temp->G > 4000 && temp->G < 4750 && temp->B > 2900 && temp->B < 3100) { //Light Blue(Turn left 135)
         for (int j = 0; j <= 20; j++) { //for 135 deg, j=20, for 90 deg, j=15
         turnLeft(mL, mR);
         __delay_ms(30);
         norm_stop(mL, mR);
         __delay_ms(30);
     }
    }
    
}