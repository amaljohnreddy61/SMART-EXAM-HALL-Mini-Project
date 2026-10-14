/* keypad.c */

#include <lpc214x.h>
#include "keypad.h"
#include "delay.h"

#define R1 (1<<16)
#define R2 (1<<17)
#define R3 (1<<18)
#define R4 (1<<19)

#define C1 (1<<20)
#define C2 (1<<21)
#define C3 (1<<22)
#define C4 (1<<23)

static const char KeyMap[4][4] =
{
    {'1','2','3','A'},
    {'4','5','6','B'},
    {'7','8','9','C'},
    {'*','0','#','D'}
};

void Keypad_Init(void)
{
    IODIR1 |= (R1|R2|R3|R4);

    IODIR1 &= ~(C1|C2|C3|C4);

    IOSET1 = R1|R2|R3|R4;
}

#define KEY_DEBOUNCE_MS 20

/* Returns 1 for a genuine key press, 0 if it was only noise.
 * Waits for the key to be released and lets the contact settle again. */
static u8 Key_Confirm(u32 col)
{
    delay_ms(KEY_DEBOUNCE_MS);          /* let the press bounce settle */

    if(IOPIN1 & col)                    /* released again -> noise */
        return 0;

    while(!(IOPIN1 & col));             /* wait for release */

    delay_ms(KEY_DEBOUNCE_MS);          /* let the release bounce settle */
    return 1;
}

u8 Keypad_GetKey(void)
{
    u8 row;;

    while(1)
    {
        for(row=0; row<4; row++)
        {
            IOSET1 = R1|R2|R3|R4;

            switch(row)
            {
                case 0: IOCLR1 = R1; break;
                case 1: IOCLR1 = R2; break;
                case 2: IOCLR1 = R3; break;
                case 3: IOCLR1 = R4; break;
            }

            delay_ms(1);

            if(!(IOPIN1 & C1))
            {
                if(Key_Confirm(C1))
                    return KeyMap[row][0];
            }

            if(!(IOPIN1 & C2))
            {
                if(Key_Confirm(C2))
                    return KeyMap[row][1];
            }

            if(!(IOPIN1 & C3))
            {
                if(Key_Confirm(C3))
                    return KeyMap[row][2];
            }

            if(!(IOPIN1 & C4))
            {
                if(Key_Confirm(C4))
                    return KeyMap[row][3];
            }
        }
    }
}
