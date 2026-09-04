#ifndef __LED_H__
#define __LED_H__

#include <lpc214x.h>

#define GREEN_LED  (1<<24)
#define YELLOW_LED (1<<25)
#define RED_LED    (1<<26)

void LED_Init(void);

void Green_LED_ON(void);
void Green_LED_OFF(void);

void Yellow_LED_ON(void);
void Yellow_LED_OFF(void);

void Red_LED_ON(void);
void Red_LED_OFF(void);

void LED_Status(unsigned int mins, unsigned int total_mins);

#endif
