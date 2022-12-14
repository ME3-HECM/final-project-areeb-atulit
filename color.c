#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include <stdio.h>
#include "interrupts.h"
#include "dc_motor.h"

void color_click_init(void) {
    //setup colour sensor via i2c interface
    I2C_2_Master_Init(); //Initialise i2c Master

    //set device PON
    color_writetoaddr(0x00, 0x01);
    __delay_ms(3); //need to wait 3ms for everthing to start up

    //turn on device ADC
    color_writetoaddr(0x00, 0x03);

    //set integration time
    color_writetoaddr(0x01, 0xD5);
}

void color_writetoaddr(char address, char value) {
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address); //command + register address
    I2C_2_Master_Write(value);
    I2C_2_Master_Stop(); //Stop condition
}

unsigned int color_read_Red(void) {
    unsigned int tmp;
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit address + Write mode
    I2C_2_Master_Write(0xA0 | 0x16); //command (auto-increment protocol transaction) + start at RED low register
    I2C_2_Master_RepStart(); // start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01); //7 bit address + Read (1) mode
    tmp = I2C_2_Master_Read(1); //read the Red LSB
    tmp = tmp | (I2C_2_Master_Read(0) << 8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); //Stop condition
    return tmp;
}

unsigned int color_read_Green(void) {
    unsigned int tmp;
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit address + Write mode
    I2C_2_Master_Write(0xA0 | 0x18); //command (auto-increment protocol transaction) + start at RED low register
    I2C_2_Master_RepStart(); // start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01); //7 bit address + Read (1) mode
    tmp = I2C_2_Master_Read(1); //read the Red LSB
    tmp = tmp | (I2C_2_Master_Read(0) << 8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); //Stop condition
    return tmp;
}

unsigned int color_read_Blue(void) {
    unsigned int tmp;
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit address + Write mode
    I2C_2_Master_Write(0xA0 | 0x1A); //command (auto-increment protocol transaction) + start at RED low register
    I2C_2_Master_RepStart(); // start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01); //7 bit address + Read (1) mode
    tmp = I2C_2_Master_Read(1); //read the Red LSB
    tmp = tmp | (I2C_2_Master_Read(0) << 8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); //Stop condition
    return tmp;
}

unsigned int color_read_Clear(void) {
    unsigned int tmp;
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit address + Write mode
    I2C_2_Master_Write(0xA0 | 0x14); //command (auto-increment protocol transaction) + start at RED low register
    I2C_2_Master_RepStart(); // start a repeated transmission
    I2C_2_Master_Write(0x52 | 0x01); //7 bit address + Read (1) mode
    tmp = I2C_2_Master_Read(1); //read the Red LSB
    tmp = tmp | (I2C_2_Master_Read(0) << 8); //read the Red MSB (don't acknowledge as this is the last read)
    I2C_2_Master_Stop(); //Stop condition
    return tmp;
}

//function to send RGBC values to serial simultaneously
void RGBC2Serial(char *str) {
    __delay_ms(200);
    sendStringSerial4(str);
}

//function to read one set of RGBC values
void color_read_RGBC(struct RGBC_val *temp) {
    temp->R = color_read_Red();
    temp->B = color_read_Blue();
    temp->G = color_read_Green();
    temp->C = color_read_Clear();
}

//normalise color values to standardise across ambient conditions. 
void color_normalise(struct RGBC_val *RGBC) {
    amb_clear = 2385;
    RGBC->norm_R = RGBC->C / RGBC->R;
    RGBC->norm_G = RGBC->C / RGBC->G;
    RGBC->norm_B = RGBC->C / RGBC->B;
    RGBC->norm_C = RGBC->C / amb_clear;
}

//function to format RGBC values into a string and send to realterm
char colorVal2String(char *buf, struct RGBC_val *temp) {
    sprintf(buf, "RGBC:%f %f %f %f \n", temp->norm_R, temp->norm_G, temp->norm_B, temp->norm_C);
    //sprintf(buf,"norm_RGBC:%i %i %i %i\n",temp->norm_R, temp->norm_G, temp->norm_B, temp->norm_C);
    return buf;
}

//turn on all three LEDs of the tricolour LED
void tricolorLED(void) {
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    rLED();
    gLED();
    bLED();
}

// turn on red led on color click
void rLED(void) {
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 1; //turns on red
}

// turn on green led on color click
void gLED(void) {
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 1; //turn on green
}

// turn on green led on color click
void bLED(void) {
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1; // turn on blue
}

// turn of tricolor led
void tricolorLEDoff(void) {
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
  LATAbits.LATA3 = 0;//turn off blue
  LATEbits.LATE7 = 0;//turn off red
  LATEbits.LATE7 = 0; //turn off green
}

//function to determine colour read by buggy and perform appropriate response. 
char motor_response(struct RGBC_val *temp, struct DC_motor *mL, struct DC_motor *mR) {
        if (temp->norm_C <9 && temp->norm_C >6){
            //Pink(Reverse 1 square and turn left 90)
         if (temp->norm_B < 5 && temp->norm_R > 1.7 && temp->norm_R < 2.2 && temp->norm_G > 3 && temp->norm_G < 3.5) { 
             reverse(mL, mR);
        __delay_ms(3000);
        norm_stop(mL, mR);
        __delay_ms(100);
        turnLeft(mL, mR);
        __delay_ms(250);
        return 5;
    }
         //Orange(Turn Right 135)
         if (temp->norm_B > 5.5 && temp->norm_B < 6 && temp->norm_R > 1.4 && temp->norm_R < 1.7 && temp->norm_G > 4.1 && temp->norm_G < 4.5) { 
        //        LATHbits.LATH0 = !LATHbits.LATH0; 
        turnPrep(mL, mR);
        turnRight(mL, mR);
        __delay_ms(430);
        return 6;
    }   //Light Blue(Turn left 135)
         if (temp->norm_B > 3.3 && temp->norm_B < 3.9 && temp->norm_R > 2.7 && temp->norm_R < 3.2 && temp->norm_G > 2.5 && temp->norm_G < 2.9) { 
        LATFbits.LATF0 = !LATFbits.LATF0;
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(430);
        norm_stop(mL, mR);
        __delay_ms(1000);
        return 7;
    }
    
    }
      
    
    if (temp->norm_C <6 && temp->norm_C >1.5){
      if (temp->norm_G > 8) { //Red Colour (Turn 90deg Right)
        turnPrep(mL, mR);
        turnRight(mL, mR);
        __delay_ms(280);
        return 1;
    }
    if (temp->norm_B > 4.5 && temp->norm_B < 5.5) { //Green Colour (Turn 90deg Left)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(270);
        return 2;
    }
    if (temp->norm_B > 2.7 && temp->norm_B < 3.3 && temp->norm_R > 2.7 && temp->norm_R < 3.3 && temp->norm_G > 2.7 && temp->norm_G < 3.3) { //Dark Blue (Turn left 180)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(485);
        return 3;
    }
    }    
    if (temp->norm_C >7){
        if (temp->norm_B > 5) { //Yellow(Reverse 1 square and turn right 90)
        reverse(mL, mR);
        __delay_ms(3000);
        norm_stop(mL, mR);
        __delay_ms(100);
        turnRight(mL, mR);
        __delay_ms(290);
        return 4;
    }
//        if (temp->norm_B < 5 && temp->norm_R > 1.7 && temp->norm_R < 2.2 && temp->norm_G > 3 && temp->norm_G < 3.5) { //Pink(Reverse 1 square and turn left 90)
//        LATHbits.LATH0 = !LATHbits.LATH0;
//        reverse(mL, mR);
//        __delay_ms(2000);
//        norm_stop(mL, mR);
//        __delay_ms(300);
//        turnLeft(mL, mR);
//        __delay_ms(250);
//        return 5;
//    }
        if (temp->norm_B < 5) { //White (Turn left 180 and return)

        motor_return = 1;
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(470);
        return 8;
        }
    }
 else { //black (Turn 360 degrees slowly, retrace if not interrupted)
        if(lost_ctr<4){
            lost_ctr++;
        }
        else{
        norm_stop(mL, mR); 
        __delay_ms(80);
        for (int j = 0; j <= 90; j++) { //for 135 deg, j=20, for 90 deg, j=15
            turnLeft(mL, mR);
            __delay_ms(30);
            norm_stop(mL, mR);
            __delay_ms(60);
        }
        lost_ctr=0;
        }
        if (interrupt_flag == 0) {
            motor_return = 1;
        }
        buggy_step --;
        return 9;

 }}

    void motor_retrace(char *buggy_path, struct DC_motor *mL, struct DC_motor * mR) {
        if (buggy_path[buggy_step - 2] == 1) { //Red Colour (Turn 90deg Left)
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(280);

        } else if (buggy_path[buggy_step - 2] == 2) { //Green Colour (Turn 90deg Right)
            turnPrep(mL, mR);
            turnRight(mL, mR);
            __delay_ms(280);

        } else if (buggy_path[buggy_step - 2] == 3) { //Dark Blue (Turn left 180)
            turnPrep(mL, mR);
            turnRight(mL, mR);
            __delay_ms(495);

        } else if (buggy_path[buggy_step - 2] == 4) { //Yellow(Reverse 1 square and turn Left 90)
            turnPrep(mL,mR);
            turnRight(mL, mR);
            __delay_ms(280);
            norm_stop(mL, mR);
            __delay_ms(100);
            fullSpeedAhead(mL, mR);
            __delay_ms(1200);
            reverse(mL, mR);
            __delay_ms(600);
            turnLeft(mL, mR);
            __delay_ms(285);
            norm_stop(mL, mR);
            __delay_ms(100);

        } else if (buggy_path[buggy_step - 2] == 5) { //Pink(Reverse 1 square and turn left 90)
            norm_stop(mL, mR);
            __delay_ms(100);
            turnLeft(mL, mR);
            __delay_ms(250);
            norm_stop(mL, mR);
            __delay_ms(100);
            fullSpeedAhead(mL, mR);
            __delay_ms(2000);

        } else if (buggy_path[buggy_step - 2] == 6) { //Orange(Turn Right 135)
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(350);
        } else if (buggy_path[buggy_step - 2] == 7) { //Light Blue(Turn left 135)
            turnPrep(mL, mR);
            turnRight(mL, mR);
            __delay_ms(350);
        }


    }    
