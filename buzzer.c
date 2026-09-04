
/* buzzer.c */

#include <lpc214x.h>
#include "buzzer.h"
#include "delay.h"

void Buzzer_Init(void)
{
    IODIR1 |= BUZZER;
    IOCLR1  = BUZZER;
}

void Buzzer_ON(void)
{
    IOSET1 = BUZZER;
}

void Buzzer_OFF(void)
{
    IOCLR1 = BUZZER;
}

void Buzzer_Beep(unsigned int ms)
{
    Buzzer_ON();
    delay_ms(ms);
    Buzzer_OFF();
}
