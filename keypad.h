#ifndef __KEYPAD_H__
#define __KEYPAD_H__

#include <lpc214x.h>
#include "types.h"

void Keypad_Init(void);
u8 Keypad_GetKey(void);

#endif
