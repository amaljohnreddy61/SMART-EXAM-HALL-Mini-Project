#ifndef __LCD_H__
#define __LCD_H__

#include <lpc214x.h>
#include "types.h"
#define LCD_D  (0xFF << 4)   /* P0.4-P0.11 : D0-D7 */

#define RS (1<<12)           /* P0.12 */
#define EN (1<<13)           /* P0.13 */

void LCD_Init(void);
void LCD_Command(u8 cmd);
void LCD_Data(u8 data);
void LCD_String(const char *str);
void LCD_Clear(void);
void LCD_GotoXY(u8 row,u8 col);
void LCD_Integer(s32 num);

#endif
