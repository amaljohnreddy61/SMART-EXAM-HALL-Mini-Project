#ifndef __BUZZER_H__
#define __BUZZER_H__

#include <lpc214x.h>

#define BUZZER (1<<27)

void Buzzer_Init(void);

void Buzzer_ON(void);

void Buzzer_OFF(void);

void Buzzer_Beep(unsigned int ms);

#endif
