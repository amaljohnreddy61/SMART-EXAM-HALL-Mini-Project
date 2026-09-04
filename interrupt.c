/* interrupt.c */

#include <lpc214x.h>
#include "interrupt.h"
#include "delay.h"

volatile unsigned char Edit_Request  = 0;
volatile unsigned char Pause_Request = 0;

void Interrupt_Init(void)
{
    /* Configure P0.1 as EINT0 and P0.3 as EINT1 (original, simplest pins) */

		PINSEL0 &= ~(0x0000000F);
		PINSEL0 |= 0x000000CC;

    EXTMODE  = 0x03;     /* EINT0 & EINT1 edge triggered */
    EXTPOLAR = 0x00;     /* Falling edge */

    EXTINT = 0x03;       /* Clear pending interrupts */

    VICVectAddr0 = (unsigned long)EINT0_ISR;
    VICVectCntl0 = 0x20 | 14;

    VICVectAddr1 = (unsigned long)EINT1_ISR;
    VICVectCntl1 = 0x20 | 15;

    VICIntEnable |= (1 << 14);
    VICIntEnable |= (1 << 15);
}


#define DEBOUNCE_US 2000   /* shortened from 20ms for a snappier response */

void EINT0_ISR(void)__irq
{
    delay_us(DEBOUNCE_US);

    if(!(IOPIN0 & (1 << 1)))   /* P0.1 still low -> genuine press */
    {
        Edit_Request = 1;
    }

    EXTINT = 0x01;

    VICVectAddr = 0;
}

void EINT1_ISR(void)__irq
{
    delay_us(DEBOUNCE_US);

    if(!(IOPIN0 & (1 << 3)))   /* P0.3 still low -> genuine press */
    {
        Pause_Request ^= 1;
    }

    EXTINT = 0x02;

    VICVectAddr = 0;
}
