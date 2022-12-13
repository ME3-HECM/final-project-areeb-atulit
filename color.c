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

void RGBC2Serial(char *str) {
    __delay_ms(200);
    sendStringSerial4(str);
}

void color_read_RGBC(struct RGBC_val *temp) {
    temp->R = color_read_Red();
    temp->B = color_read_Blue();
    temp->G = color_read_Green();
    temp->C = color_read_Clear();
}

void color_normalise(struct RGBC_val *RGBC) {
    RGBC->norm_R = RGBC->C / RGBC->R;
    RGBC->norm_G = RGBC->C / RGBC->G;
    RGBC->norm_B = RGBC->C / RGBC->B;
    RGBC->norm_C = RGBC->C / 2650;
}

char colorVal2String(char *buf, struct RGBC_val *temp) {
    sprintf(buf, "RGBC:%i %i %i %i %i\n", temp->R, temp->G, temp->B, temp->C, temp->norm_R);
    //sprintf(buf,"norm_RGBC:%f %f %f %f\n",temp->norm_R, temp->norm_G, temp->norm_B, temp->norm_C);
    return buf;
}

void tricolorLED(void) {
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 1;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 1;
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 1;
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1;
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 1;
}

void tricolorLEDoff(void) {
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 0;
    TRISAbits.TRISA4 = 0;
    LATAbits.LATA4 = 0;
    TRISFbits.TRISF7 = 0;
    LATFbits.LATF7 = 0;
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 0;
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 0;
}

char motor_response(struct RGBC_val *temp, struct DC_motor *mL, struct DC_motor *mR) {

    if (temp->norm_C < 6 && temp-> norm_C > 1.5) {
        if (temp->norm_G > 8) { //Red Colour (Turn 90deg Right)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnRight(mL, mR);
            __delay_ms(220);
            return 1;
        } 
        else if (temp->norm_B > 4.5 && temp->norm_B < 5.5) { //Green Colour (Turn 90deg Left)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnLeft(mL, mR);
            __delay_ms(250);
            return 2;
        } 
        else if (temp->norm_B > 2.8 && temp->norm_B < 3.2 && temp->norm_R > 2.8 && temp->norm_R < 3.2 && temp->norm_G > 2.8 && temp->norm_G < 3.2) { //Dark Blue (Turn left 180)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnLeft(mL, mR);
            __delay_ms(410);
            return 3;
        }
    }
    else if (temp->norm_C > 6.01 && temp->norm_C < 9) {
            if (temp->norm_B < 5 && temp->norm_R > 1.7 && temp->norm_R < 2.2 && temp->norm_G > 3 && temp->norm_G < 3.5) { //Pink(Reverse 1 square and turn left 90)
               LATHbits.LATH0 = !LATHbits.LATH0;
                reverse(mL, mR);
                __delay_ms(2000);
                norm_stop(mL, mR);
                __delay_ms(300);
                turnLeft(mL, mR);
                __delay_ms(250);
                return 5;
            } else if (temp->norm_B > 5.5 && temp->norm_B < 6 && temp->norm_R > 1.4 && temp->norm_R < 1.7 && temp->norm_G > 4.1 && temp->norm_G < 4.5) { //Orange(Turn Right 135)
                //        LATHbits.LATH0 = !LATHbits.LATH0; 
                turnRight(mL, mR);
                __delay_ms(350);
                return 6;
            } else if (temp->norm_B > 3.3 && temp->norm_B < 3.9 && temp->norm_R > 2.7 && temp->norm_R < 3.2 && temp->norm_G > 2.5 && temp->norm_G < 2.9) { //Light Blue(Turn left 135)
                LATFbits.LATF0 = !LATFbits.LATF0;
                turnLeft(mL, mR);
                __delay_ms(300);
                norm_stop(mL, mR);
                __delay_ms(2000);
                return 7;
            }
        }
        else if (temp->norm_C > 9.01) {
            //&& temp->norm_B < 6 && temp->norm_R > 1.5 && temp->norm_R < 2 && temp->norm_G > 3 && temp->norm_G < 3.5
//            if (temp->B < 5500) { //Yellow(Reverse 1 square and turn right 90)
//                reverse(mL, mR);
//                __delay_ms(2000);
//                norm_stop(mL, mR);
//                __delay_ms(100);
//                turnRight(mL, mR);
//                __delay_ms(220);
//                return 4;
//
//            }
            if (temp->norm_B > 5) { //White (Turn left 180 and return)
                
                motor_return = 1;
                reverse(mL, mR);
                __delay_ms(400);
                norm_stop(mL, mR);
                __delay_ms(80);
                turnLeft(mL, mR);
                __delay_ms(425);
                return 8;

            }
    }

            //    else { //black (Turn 360 degrees slowly, retrace if not interrupted)
            //        LATHbits.LATH0 = !LATHbits.LATH0;
            //        norm_stop(mL, mR);
            //        __delay_ms(80);
            //        for (int j = 0; j <= 45; j++) { //for 135 deg, j=20, for 90 deg, j=15
            //            turnLeft(mL, mR);
            //            __delay_ms(80);
            //            norm_stop(mL, mR);
            //            __delay_ms(30);
            //        }
            //        if(interrupt_flag==0){
            //            motor_return = 1;
            //        }
            //        fullSpeedAhead(&mL, &mR);
            //        ctr=ctr-1;

            //        return 9;

            //}
        }
    void motor_retrace(char *buggy_path, struct DC_motor *mL, struct DC_motor * mR) {
        if (buggy_path[ctr - 2] == 1) { //Red Colour (Turn 90deg Left)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnLeft(mL, mR);
            __delay_ms(250);

        } else if (buggy_path[ctr - 2] == 2) { //Green Colour (Turn 90deg Right)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnRight(mL, mR);
            __delay_ms(220);

        } else if (buggy_path[ctr - 2] == 3) { //Dark Blue (Turn left 180)
            reverse(mL, mR);
            __delay_ms(400);
            norm_stop(mL, mR);
            __delay_ms(80);
            turnRight(mL, mR);
            __delay_ms(430);

        } else if (buggy_path[ctr - 2] == 4) { //Yellow(Reverse 1 square and turn Left 90)
            norm_stop(mL, mR);
            __delay_ms(100);
            turnRight(mL, mR);
            __delay_ms(250);
            norm_stop(mL, mR);
            __delay_ms(100);
            fullSpeedAhead(mL, mR);
            __delay_ms(2000);


        } else if (buggy_path[ctr - 2] == 5) { //Pink(Reverse 1 square and turn left 90)
            norm_stop(mL, mR);
            __delay_ms(100);
            turnLeft(mL, mR);
            __delay_ms(250);
            norm_stop(mL, mR);
            __delay_ms(100);
            fullSpeedAhead(mL, mR);
            __delay_ms(2000);

        } else if (buggy_path[ctr - 2] == 6) { //Orange(Turn Right 135)
            turnLeft(mL, mR);
            __delay_ms(350);
        } else if (buggy_path[ctr - 2] == 7) { //Light Blue(Turn left 135)
            turnRight(mL, mR);
            __delay_ms(350);
        }


    }
