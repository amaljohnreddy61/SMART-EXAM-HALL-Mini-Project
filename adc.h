#ifndef __ADC_H__
#define __ADC_H__

#include <lpc214x.h>
#include "types.h"

void ADC_Init(void);

u16 ADC_Read(u8 channel);

float GetTemperature(void);

#endif
