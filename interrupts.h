#ifndef _interrupts_H
#define _interrupts_H

#include <xc.h>
#include "i2c.h"

#define _XTAL_FREQ 64000000
int interrupt_flag = 1;

void Interrupts_init(void);
void __interrupt(high_priority) HighISR();
void Color_Interrupts_init(void);
void Color_Interrupts_threshold(int upperThreshold,int lowerThreshold);
void persistence_register(void);
void Color_Interrupts_clear(void);

#endif
