#include <string.h>

#include "password.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"
#include "buzzer.h"

static char Password[PASSWORD_LENGTH + 1] = "1234";

u8 VerifyPassword(void)
{
    char entered[5];
    u8 i;

    LCD_Clear();
    LCD_String("Enter Password");

    LCD_GotoXY(1,0);

    for(i=0;i<PASSWORD_LENGTH;i++)
    {
        entered[i] = Keypad_GetKey();
        LCD_Data('*');
    }

    entered[4] = '\0';

    if(strcmp(entered,Password) == 0)
    {
        LCD_Clear();
        LCD_String("Access Granted");
        delay_ms(1000);
        return 1;
    }

    LCD_Clear();
    LCD_String("Access Denied");

    Buzzer_Beep(2000);

    delay_ms(1000);

    return 0;
}

void ChangePassword(void)
{
    char newpass[5];
    u8 i;

    LCD_Clear();
    LCD_String("New Password");

    LCD_GotoXY(1,0);

    for(i=0;i<PASSWORD_LENGTH;i++)
    {
        newpass[i] = Keypad_GetKey();
        LCD_Data('*');
    }

    newpass[4] = '\0';

    strcpy(Password,newpass);

    LCD_Clear();
    LCD_String("Pwd Updated");

    delay_ms(1000);
}
