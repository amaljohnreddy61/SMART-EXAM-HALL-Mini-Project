#ifndef __SEVENSEG_H__
#define __SEVENSEG_H__

#include <lpc214x.h>
#include "types.h"

void SevenSeg_Init(void);
void SevenSeg_Display(u8 value);

/* Blank the display (both digits fully off) - used when no exam is
 * running, so nothing shows at power-on / idle. */
void SevenSeg_Off(void);

/* Un-blank the display - the value last set with SevenSeg_Display()
 * will start showing again, refreshed by the Timer0 ISR. */
void SevenSeg_On(void);

#endif
