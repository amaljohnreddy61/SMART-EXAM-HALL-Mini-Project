/* sevenseg.c */

#include <lpc214x.h>
#include "sevenseg.h"
#include "delay.h"
#include "types.h"

#define DIGIT1   (1<<0)   
#define DIGIT2   (1<<15)   

#define SEG_A    (1<<16)
#define SEG_B    (1<<17)
#define SEG_C    (1<<18)
#define SEG_D    (1<<19)
#define SEG_E    (1<<20)
#define SEG_F    (1<<21)
#define SEG_G    (1<<22)
#define SEG_DP   (1<<23)

static const u8 SegCode[10] =
{
    0xC0,
    0xF9,
    0xA4,
    0xB0,
    0x99,
    0x92,
    0x82,
    0xF8,
    0x80,
    0x90
};

/* Value shown on the display and on/off flag (updated by main code,
 * used by the Timer0 interrupt that refreshes the digits) */
static volatile u8 SegValue   = 0;
static volatile u8 SegEnabled = 0;

static void Timer0_ISR(void) __irq;

static void Display_Digit(u8 num)
{
    if(num > 9)
        return;

    /* Turn OFF all segments */
    IOSET0 = SEG_A | SEG_B | SEG_C | SEG_D |
             SEG_E | SEG_F | SEG_G | SEG_DP;

    if(!(SegCode[num] & 0x01)) IOCLR0 = SEG_A;
    if(!(SegCode[num] & 0x02)) IOCLR0 = SEG_B;
    if(!(SegCode[num] & 0x04)) IOCLR0 = SEG_C;
    if(!(SegCode[num] & 0x08)) IOCLR0 = SEG_D;
    if(!(SegCode[num] & 0x10)) IOCLR0 = SEG_E;
    if(!(SegCode[num] & 0x20)) IOCLR0 = SEG_F;
    if(!(SegCode[num] & 0x40)) IOCLR0 = SEG_G;

    IOSET0 = SEG_DP;
}

void SevenSeg_Init(void)
{
    IODIR0 |= DIGIT1 | DIGIT2;

    IODIR0 |= SEG_A | SEG_B | SEG_C | SEG_D |
              SEG_E | SEG_F | SEG_G | SEG_DP;

    /* Disable both digits */
    IOCLR0 = DIGIT1 | DIGIT2;

    /* Turn OFF all segments */
    IOSET0 = SEG_A | SEG_B | SEG_C | SEG_D |
             SEG_E | SEG_F | SEG_G | SEG_DP;

    /* Timer0: interrupt every 2 ms (PCLK = 15 MHz) to refresh the digits */
    T0TCR = 0x02;                 /* stop and reset timer */
    T0PR  = 0;                    /* no prescaler */
    T0MR0 = 30000 - 1;            /* 2 ms */
    T0MCR = 0x03;                 /* interrupt + reset on MR0 */
    T0IR  = 0xFF;                 /* clear pending flags */

    VICVectAddr2 = (unsigned long)Timer0_ISR;
    VICVectCntl2 = 0x20 | 4;      /* slot 2, Timer0 */
    VICIntEnable = (1 << 4);

    T0TCR = 0x01;                 /* start timer */
}

static void Timer0_ISR(void) __irq
{
    static u8 digit = 0;

    /* Blank both digits before changing segments (no ghosting) */
    IOCLR0 = DIGIT1 | DIGIT2;

    if(SegEnabled)
    {
        if(digit == 0)
        {
            Display_Digit(SegValue / 10);
            IOSET0 = DIGIT1;
        }
        else
        {
            Display_Digit(SegValue % 10);
            IOSET0 = DIGIT2;
        }
    }

    digit ^= 1;

    T0IR = 0x01;                  /* clear MR0 interrupt flag */
    VICVectAddr = 0;
}

void SevenSeg_Multiplex(u8 tens, u8 units)
{
    /* Disable both digits */
    IOCLR0 = DIGIT1 | DIGIT2;

    Display_Digit(tens);

    IOSET0 = DIGIT1;
    delay_ms(2);
    IOCLR0 = DIGIT1;

    Display_Digit(units);

    IOSET0 = DIGIT2;
    delay_ms(2);
    IOCLR0 = DIGIT2;
}

void SevenSeg_Display(u8 value)
{
    if(value > 99)
        value = 99;

    SegValue   = value;
    SegEnabled = 1;      /* display stays on; refreshed by Timer0 */
}
