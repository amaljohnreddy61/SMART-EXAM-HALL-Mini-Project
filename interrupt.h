#ifndef __INTERRUPT_H__
#define __INTERRUPT_H__

#include <lpc214x.h>

extern volatile unsigned char Edit_Request;

extern volatile unsigned char Pause_Request;

void Interrupt_Init(void);

void EINT0_ISR(void)__irq;

void EINT1_ISR(void)__irq;

#endif
