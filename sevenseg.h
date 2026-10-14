#ifndef __SEVENSEG_H__
#define __SEVENSEG_H__

#include <lpc214x.h>
#include "types.h"

void SevenSeg_Init(void);
void SevenSeg_Display(u8 value);
void SevenSeg_Multiplex(u8 tens, u8 units);
#endif
