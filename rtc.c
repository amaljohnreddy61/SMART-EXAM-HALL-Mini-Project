/* rtc.c */

#include <lpc214x.h>
#include "rtc.h"
#include "lcd.h"
#include "keypad.h"
#include "delay.h"

/* Battery-backed general purpose register - survives a reset/power
 * cycle as long as VBAT is held, so it's used here to detect whether
 * the RTC was already set up on a previous run and skip re-stamping
 * it back to the default date/time every time the board resets. */
#define GPREG0 (*(volatile unsigned long *)0xE0024044)
#define RTC_INIT_MAGIC 0xA55A

void RTC_Init(void)
{
    CCR = 0x02;

    PREINT  = 456;
    PREFRAC = 25024;

    if(GPREG0 == RTC_INIT_MAGIC &&
       SEC <= 59 && MIN <= 59 && HOUR <= 23 &&
       DOM >= 1 && DOM <= 31 && MONTH >= 1 && MONTH <= 12)
    {
        /* Already initialised on a previous run and the clock
         * registers still hold sane values - just resume ticking. */
        CCR |= 0x01;
        return;
    }

    SEC   = 0;
    MIN   = 0;
    HOUR  = 12;

    DOM   = 04;
    MONTH = 8;
    YEAR  = 2026;

    CCR = 0x01;

    GPREG0 = RTC_INIT_MAGIC;
}

void RTC_GetTime(RTC_TIME *t)
{
    t->hour = HOUR;
    t->min  = MIN;
    t->sec  = SEC;
}

void RTC_SetTime(RTC_TIME *t)
{
    HOUR = t->hour;
    MIN  = t->min;
    SEC  = t->sec;
}

void RTC_GetDate(RTC_DATE *d)
{
    d->date  = DOM;
    d->month = MONTH;
    d->year  = YEAR;
}

void RTC_SetDate(RTC_DATE *d)
{
    DOM   = d->date;
    MONTH = d->month;
    YEAR  = d->year;
}

/* Read two digits (00-99) */
static unsigned char ReadNumber(void)
{
    char ch1, ch2;

    do
    {
        ch1 = Keypad_GetKey();
    }while(ch1<'0' || ch1>'9');

    LCD_Data(ch1);

    do
    {
        ch2 = Keypad_GetKey();
    }while(ch2<'0' || ch2>'9');

    LCD_Data(ch2);

    return ((ch1-'0')*10 + (ch2-'0'));
}

/* Read four digit year */
static unsigned int ReadYear(void)
{
    char d1,d2,d3,d4;

    do{ d1=Keypad_GetKey(); }while(d1<'0'||d1>'9');
    LCD_Data(d1);

    do{ d2=Keypad_GetKey(); }while(d2<'0'||d2>'9');
    LCD_Data(d2);

    do{ d3=Keypad_GetKey(); }while(d3<'0'||d3>'9');
    LCD_Data(d3);

    do{ d4=Keypad_GetKey(); }while(d4<'0'||d4>'9');
    LCD_Data(d4);

    return (d1-'0')*1000 +
           (d2-'0')*100 +
           (d3-'0')*10 +
           (d4-'0');
}

void RTC_Edit(void)
{
    char key;
    RTC_TIME t;
    RTC_DATE d;

    RTC_GetTime(&t);
    RTC_GetDate(&d);

    while(1)
    {
        LCD_Clear();
        LCD_String("1TIME 2DATE");
        LCD_GotoXY(1,0);
        LCD_String("3SAVE #EXIT");

        key = Keypad_GetKey();

        switch(key)
        {
            case '1':

                LCD_Clear();
                LCD_String("Hour:");
                LCD_GotoXY(1,0);

                while(1)
                {
                    t.hour = ReadNumber();
                    if(t.hour<=23)
                        break;

                    LCD_Clear();
                    LCD_String("0-23 Only");
                    delay_ms(1000);

                    LCD_Clear();
                    LCD_String("Hour:");
                    LCD_GotoXY(1,0);
                }

                LCD_Clear();
                LCD_String("Minute:");
                LCD_GotoXY(1,0);

                while(1)
                {
                    t.min = ReadNumber();
                    if(t.min<=59)
                        break;

                    LCD_Clear();
                    LCD_String("0-59 Only");
                    delay_ms(1000);

                    LCD_Clear();
                    LCD_String("Minute:");
                    LCD_GotoXY(1,0);
                }

                LCD_Clear();
                LCD_String("Second:");
                LCD_GotoXY(1,0);

                while(1)
                {
                    t.sec = ReadNumber();
                    if(t.sec<=59)
                        break;

                    LCD_Clear();
                    LCD_String("0-59 Only");
                    delay_ms(1000);

                    LCD_Clear();
                    LCD_String("Second:");
                    LCD_GotoXY(1,0);
                }

                break;

            case '2':

                LCD_Clear();
                LCD_String("Date:");
                LCD_GotoXY(1,0);

                while(1)
                {
                    d.date = ReadNumber();
                    if(d.date>=1 && d.date<=31)
                        break;

                    LCD_Clear();
                    LCD_String("1-31 Only");
                    delay_ms(1000);

                    LCD_Clear();
                    LCD_String("Date:");
                    LCD_GotoXY(1,0);
                }

                LCD_Clear();
                LCD_String("Month:");
                LCD_GotoXY(1,0);

                while(1)
                {
                    d.month = ReadNumber();
                    if(d.month>=1 && d.month<=12)
                        break;

                    LCD_Clear();
                    LCD_String("1-12 Only");
                    delay_ms(1000);

                    LCD_Clear();
                    LCD_String("Month:");
                    LCD_GotoXY(1,0);
                }

                LCD_Clear();
                LCD_String("Year:");
                LCD_GotoXY(1,0);

                d.year = ReadYear();

                break;

            case '3':

                RTC_SetTime(&t);
                RTC_SetDate(&d);

                LCD_Clear();
                LCD_String("RTC Updated");
                delay_ms(1000);

                return;

            case '#':

                return;

            default:

                LCD_Clear();
                LCD_String("Invalid Key");
                delay_ms(700);
        }
    }
}