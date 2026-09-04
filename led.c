/* led.c */

#include <lpc214x.h>
#include "led.h"

void LED_Init(void)
{
    IODIR1 |= GREEN_LED | YELLOW_LED | RED_LED;

    IOCLR1 = GREEN_LED | YELLOW_LED | RED_LED;
}

void Green_LED_ON(void)
{
    IOSET1 = GREEN_LED;
}

void Green_LED_OFF(void)
{
    IOCLR1 = GREEN_LED;
}

void Yellow_LED_ON(void)
{
    IOSET1 = YELLOW_LED;
}

void Yellow_LED_OFF(void)
{
    IOCLR1 = YELLOW_LED;
}

void Red_LED_ON(void)
{
    IOSET1 = RED_LED;
}

void Red_LED_OFF(void)
{
    IOCLR1 = RED_LED;
}

void LED_Status(unsigned int mins, unsigned int total_mins)
{
    /* Turn OFF all LEDs first */
    IOCLR1 = GREEN_LED | YELLOW_LED | RED_LED;

    if(mins == 0)
    {
        return;   /* no exam running - stay off */
    }

    if(total_mins == 0)
        total_mins = 1;   /* avoid divide-by-zero */

    /* The last minute is ALWAYS red, no matter how short the exam is
     * (this matters for 1-2 minute exams, where the percentage split
     * below wouldn't otherwise leave room for every colour). */
    if(mins <= 1)
    {
        IOSET1 = RED_LED;
        return;
    }

    /* Otherwise the thresholds scale with the exam's own length, so
     * a 5-minute exam and a 90-minute exam both get a fair
     * GREEN/YELLOW split instead of the old fixed 10-minute cutoff. */
    if(mins > (total_mins * 50) / 100)
    {
        IOSET1 = GREEN_LED;
    }
    else
    {
        IOSET1 = YELLOW_LED;
    }
}
