/* delay.c */
#include <lpc214x.h>
#include "delay.h"

void delay_us(unsigned int us)
{
    unsigned int i;

    while(us--)
    {
        for(i=0;i<15;i++);
    }
}

void delay_ms(unsigned int ms)
{
    unsigned int i;
    while(ms--)
    {
        for(i=0;i<6000;i++);
    }
}
