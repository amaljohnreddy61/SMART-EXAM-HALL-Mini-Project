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
        return;
    }

    (void)total_mins;   /* thresholds are now fixed minutes, not percentages */

    if(mins > 10)
    {
        IOSET1 = GREEN_LED;
    }
    else if(mins > 1)
    {
        IOSET1 = YELLOW_LED;   /* final 10 minutes */
    }
    else
    {
        IOSET1 = RED_LED;      /* last 1 minute */
    }
}
