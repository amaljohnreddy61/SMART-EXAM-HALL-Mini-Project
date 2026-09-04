/* sevenseg.c */

#include <lpc214x.h>
#include "sevenseg.h"
#include "types.h"

/* DIGIT1 moved off P0.14: on the LPC2148 that pin is the ISP/boot-select
 * line, and most dev boards have an external pull-up on it (for the ISP
 * button) that fights a GPIO trying to drive it - that's why only the
 * right-hand digit used to light up. */
#define DIGIT1   (1<<0)    /* P0.0  */
#define DIGIT2   (1<<15)   /* P0.15 */

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

/* Value currently being shown, and whether the display should be lit
 * at all. Both are only ever written by SevenSeg_Display()/On()/Off()
 * and only ever read by the Timer0 ISR below - so the ISR always
 * multiplexes whatever the main code most recently asked for, without
 * main code ever having to drive the segment pins itself. */
static volatile u8 PendingTens   = 0;
static volatile u8 PendingUnits  = 0;
static volatile u8 DisplayActive = 0;
static volatile u8 Phase         = 0;

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

/* Timer0 ISR - fires every ~2ms and lights ONE digit per call,
 * alternating tens/units, so each digit is refreshed every ~4ms
 * (~250Hz). This runs independently of main()'s loop, so the display
 * stays rock-steady no matter how long the LCD writes or keypad wait
 * take - that's what removes the blinking/flicker for good, instead
 * of just reducing it. */
void TIMER0_ISR(void) __irq
{
    IOCLR0 = DIGIT1 | DIGIT2;

    if(DisplayActive)
    {
        if(Phase == 0)
        {
            Display_Digit(PendingTens);
            IOSET0 = DIGIT1;
        }
        else
        {
            Display_Digit(PendingUnits);
            IOSET0 = DIGIT2;
        }

        Phase ^= 1;
    }

    T0IR = 0x01;         /* clear the MR0 match interrupt flag */
    VICVectAddr = 0;     /* tell the VIC this interrupt is handled */
}

/* PCLK on this board is 15,000,000 Hz - confirmed from RTC_Init()'s
 * PREINT=456 / PREFRAC=25024 (those only give an exact 1Hz RTC tick
 * out of a 32.768kHz crystal if PCLK = (456+1)*32768 + 25024 =
 * 15,000,000). With no prescale, T0MR0=30000 gives a 2ms period. */
static void Timer0_Init(void)
{
    T0TCR = 0x02;        /* reset the timer */
    T0PR  = 0;           /* no prescale - TC counts at PCLK */
    T0MR0 = 30000;        /* 30000 / 15,000,000 Hz = 2ms */
    T0MCR = 0x03;         /* interrupt on MR0 match, reset TC on match */

    VICVectAddr2 = (unsigned long)TIMER0_ISR;
    VICVectCntl2 = 0x20 | 4;   /* Timer0 = VIC interrupt source 4 */
    VICIntEnable |= (1 << 4);

    T0TCR = 0x01;         /* start the timer */
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

    DisplayActive = 0;   /* blank until an exam actually starts */

    Timer0_Init();
}

void SevenSeg_Display(u8 value)
{
    if(value > 99)
        value = 99;

    PendingTens  = value / 10;
    PendingUnits = value % 10;
}

void SevenSeg_On(void)
{
    DisplayActive = 1;
}

void SevenSeg_Off(void)
{
    DisplayActive = 0;

    /* Blank immediately instead of waiting for the next timer tick */
    IOCLR0 = DIGIT1 | DIGIT2;
}
