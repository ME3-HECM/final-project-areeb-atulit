#include <xc.h>
#include "color.h"
#include "i2c.h"
#include "serial.h"
#include <stdio.h>
#include "interrupts.h"
#include "dc_motor.h"

/********************************************//**
 *  Function to initialise the colour click module using I2C
 ***********************************************/
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

/********************************************//**
 *  Function to write to the colour click module
 *  address is the register within the colour click to write to
 *	value is the value that will be written to that address
 ***********************************************/
void color_writetoaddr(char address, char value) {
    I2C_2_Master_Start(); //Start condition
    I2C_2_Master_Write(0x52 | 0x00); //7 bit device address + Write mode
    I2C_2_Master_Write(0x80 | address); //command + register address
    I2C_2_Master_Write(value);
    I2C_2_Master_Stop(); //Stop condition
}

/********************************************//**
 *  Function to read the red channel
 *	Returns a 16 bit ADC value representing colour intensity
 ***********************************************/
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

/**********************************************
 *  Function to send RGBC values to serial simultaneously
 ***********************************************/
void RGBC2Serial(char *str) {
    __delay_ms(200);
    sendStringSerial4(str);
}

/**********************************************
 *  Function to read one set of RGBC values
 ***********************************************/
void color_read_RGBC(struct RGBC_val *temp) {
    
    temp->R = color_read_Red();
    temp->B = color_read_Blue();
    temp->G = color_read_Green();
    temp->C = color_read_Clear();
    
}


/********************************************************************
 *  Normalise color values to standardise across ambient conditions. 
 *******************************************************************/
void color_normalise(struct RGBC_val *RGBC) {
    amb_clear = 2385;
    RGBC->norm_R = RGBC->C / RGBC->R;
    RGBC->norm_G = RGBC->C / RGBC->G;
    RGBC->norm_B = RGBC->C / RGBC->B;
    RGBC->norm_C = RGBC->C / amb_clear;
}


/********************************************************************
 *  Function to format RGBC values into a string and send to realterm
 *******************************************************************/
char colorVal2String(char *buf, struct RGBC_val *temp) {
    //sprintf(buf, "RGBC:%f %f %f %f \n", temp->norm_R, temp->norm_G, temp->norm_B, temp->norm_C);
    sprintf(buf, "RGBC:%f %f %f %f %f\n", CR1L, CR2U, CR2L, CR3U, CR3L);
    //sprintf(buf,"norm_RGBC:%i %i %i %i\n",temp->norm_R, temp->norm_G, temp->norm_B, temp->norm_C);
    return buf;
}

/********************************************************************
 *  Turn on all three LEDs of the tricolour LED
 *******************************************************************/
void tricolorLED(void) {
    //to turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    rLED();
    gLED();
    bLED();
}


/********************************************************************
 * Turn on red led on color click
 *******************************************************************/
void rLED(void) {
    TRISGbits.TRISG0 = 0;
    LATGbits.LATG0 = 1; //turns on red
}

/********************************************************************
 * Turn on green led on color click
 *******************************************************************/
void gLED(void) {
    TRISEbits.TRISE7 = 0;
    LATEbits.LATE7 = 1; //turn on green
}

/********************************************************************
 * Turn on  blue led on color click
 *******************************************************************/
void bLED(void) {
    TRISAbits.TRISA3 = 0;
    LATAbits.LATA3 = 1; // turn on blue
}

/********************************************************************
 * Turn off tricolor led on color click
 *******************************************************************/
void tricolorLEDoff(void) {
    //To turn on rgb pins on colour clicker, check microbus pin, map to buggy pin and then pic pin
    LATAbits.LATA3 = 0; //turn off blue
    LATEbits.LATE7 = 0; //turn off red
    LATEbits.LATE7 = 0; //turn off green
}


/******************************************************************************
 * Function to read clear value for colours and create clear ranges accordingly
 * Read yellow, pink, light blue, green, dark blue in order specified.
 *****************************************************************************/
void rangeCalibrate(struct RGBC_val *RGBC, struct DC_motor *mL,struct DC_motor *mR ) {
    float clearArr[6]; //Create a float array
    for (int calibCtr=0;calibCtr < 6;){
        if (!PORTFbits.RF2) { //Check if switch is on
            if(calibCtr!=5){ //press against wall if reading colour, not if reading ambient
            wallSmash(mL, mR);}
            __delay_ms(500);
            LATHbits.LATH3 = 1;
            color_read_RGBC(RGBC);
            color_normalise(RGBC);
            __delay_ms(500);
            LATHbits.LATH3 = 0;
            clearArr[calibCtr] = RGBC->norm_C;//add normalised clear for given colour into array
            __delay_ms(500);
            norm_stop(mL, mR);
            __delay_ms(500);
            calibCtr++;
        }
    }
    //assign upper and lower limits for each clear value range with appropriate tolerances
            CR1L = clearArr[0]-0.4;
            CR2U = clearArr[1]+0.3;
            CR2L = clearArr[2]-0.3;
            CR3U = clearArr[3]+0.4;
            CR3L = clearArr[4]-0.2;
            __delay_ms(2000);
}


/******************************************************************************
 * Function to determine colour read by buggy and perform appropriate response.
 * The function seperates the colours first into 3 seperate groups using the normalised clear values.
 * The normalised RGB values are then used to distinguish between colours in each group.
 *****************************************************************************/
char motor_response(struct RGBC_val *temp, struct DC_motor *mL, struct DC_motor *mR) {
    
    if (temp->norm_C < CR2U && temp->norm_C > CR2L) {//6&9 for buggy 1, 3.5 and 5.5 buggy 2
        //Pink(Reverse 1 square and turn left 90)
        if (temp->norm_B < 5 && temp->norm_R > 1.7 && temp->norm_R < 2.2 && temp->norm_G > 3 && temp->norm_G < 3.5) {
            reverse(mL, mR);
            __delay_ms(3000);
            norm_stop(mL, mR);
            __delay_ms(100);
            turnLeft(mL, mR);
            __delay_ms(210);
            return 5;
        }
        //Orange(Turn Right 135)
        if (temp->norm_B > 5.5 && temp->norm_B < 6 && temp->norm_R > 1.4 && temp->norm_R < 1.7 && temp->norm_G > 4.1 && temp->norm_G < 4.5) {
            turnPrep(mL, mR);
            turnRight(mL, mR);
            __delay_ms(325);
            norm_stop(mL, mR);
            __delay_ms(1000);
            return 6;
        } //Light Blue(Turn left 135)
        if (temp->norm_B > 3.3 && temp->norm_B < 3.9 && temp->norm_R > 2.7 && temp->norm_R < 3.2 && temp->norm_G > 2.5 && temp->norm_G < 2.9) {
            LATFbits.LATF0 = !LATFbits.LATF0;
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(330);
            norm_stop(mL, mR);
            __delay_ms(1000);
            return 7;
        }

    }


    if (temp->norm_C < CR3U && temp->norm_C > CR3L) {//6 and 1.5 buggy 1 and 3 
        if (temp->norm_G > 8) { //Red Colour (Turn 90deg Right)
            turnPrep(mL, mR);
            turnRight(mL, mR);
            __delay_ms(225);
            return 1;
        }
        if (temp->norm_B > 4.5 && temp->norm_B < 5.5) { //Green Colour (Turn 90deg Left)
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(210);
            return 2;
        }
        if (temp->norm_B > 2.7 && temp->norm_B < 3.3 && temp->norm_R > 2.7 && temp->norm_R < 3.3 && temp->norm_G > 2.7 && temp->norm_G < 3.3) { //Dark Blue (Turn left 180)
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(385);
            return 3;
        }
    }
    if (temp->norm_C > CR1L) {//7 for buggy 1, 5.7 buggy 2
        if (temp->norm_B > 5) { //Yellow(Reverse 1 square and turn right 90)
            reverse(mL, mR);
            __delay_ms(3000);
            norm_stop(mL, mR);
            __delay_ms(100);
            turnRight(mL, mR);
            __delay_ms(205);
            return 4;
        }

        if (temp->norm_B < 5) { //White (Turn left 180 and return)

            motor_return = 1;
            LATDbits.LATD4 = 1; //turn brake lights on
            turnPrep(mL, mR);
            turnLeft(mL, mR);
            __delay_ms(385);
            LATHbits.LATH3 = 0;
            LATDbits.LATD7 = 1;
            return 8;
        }
    }
    else { //black (Turn 360 degrees slowly, retrace if not interrupted)
        if (lost_ctr < 2) {
            lost_ctr++;
        } else {
            turnPrep(mL, mR);
            //pulse in a circle looking for a colour to follow
            for (int j = 0; j <= 75; j++) { //for 135 deg, j=20, for 90 deg, j=15
                turnLeft(mL, mR);
                __delay_ms(30);
                norm_stop(mL, mR);
                __delay_ms(60);
            }
            turnLeft(mL, mR);//take a u turn
            __delay_ms(385);
            norm_stop(mL, mR);
            __delay_ms(1000);
            motor_return = 1;//set motor to return home if it fails to find a colour
            buggy_step--;
            lost_ctr = 0;//reset counter 
            return 9;
        }

    }
}

/******************************************************************************
 * Function to perform retracing which gets triggered once the interrupt has been triggered. 
 * The function checks against the step in the buggy_path array to determine which colour it last saw. 
 * Once it determines the colour, it does the opposite of what the colour initially did, to ensure retracing.
 *****************************************************************************/
void motor_retrace(char *buggy_path, struct DC_motor *mL, struct DC_motor * mR) { //Do the opposite of motor_response for each colour
    if (buggy_path[buggy_step - 2] == 1) { //Red Colour (Turn 90deg Left)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(200);

    } else if (buggy_path[buggy_step - 2] == 2) { //Green Colour (Turn 90deg Right)
        turnPrep(mL, mR);
        turnRight(mL, mR);
        __delay_ms(215);

    } else if (buggy_path[buggy_step - 2] == 3) { //Dark Blue (U Turn)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(385);

    } else if (buggy_path[buggy_step - 2] == 4) { //Yellow(Reverse 1 square and turn Left 90)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(210);
        norm_stop(mL, mR);
        __delay_ms(100);
        fullSpeedAhead(mL, mR);
        __delay_ms(1200);

    } else if (buggy_path[buggy_step - 2] == 5) { //Pink(Reverse 1 square and turn right 90)
        norm_stop(mL, mR);
        __delay_ms(100);
        turnRight(mL, mR);
        __delay_ms(210);
        norm_stop(mL, mR);
        __delay_ms(100);
        fullSpeedAhead(mL, mR);
        __delay_ms(2000);

    } else if (buggy_path[buggy_step - 2] == 6) { //Orange(Turn Left 135)
        turnPrep(mL, mR);
        turnLeft(mL, mR);
        __delay_ms(330);
        
    } else if (buggy_path[buggy_step - 2] == 7) { //Light Blue(Turn Right 135)
        turnPrep(mL, mR);
        turnRight(mL, mR);
        __delay_ms(325);
    }


}
//
/******************************************************************************
 * Function to initialise switch for color calibration 
 *****************************************************************************/
void calibSwitchInit(void){
    TRISFbits.TRISF2 = 1; //set TRIS value for pin (input)
    ANSELFbits.ANSELF2 = 0; //turn off analogue input on pin
}
