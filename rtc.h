#ifndef __RTC_H__
#define __RTC_H__

#include <lpc214x.h>
#include "types.h"

typedef struct rtc
{
    u8 hour;
    u8 min;
    u8 sec;

}RTC_TIME;

typedef struct rtc1
{
    u8 date;
    u8 month;
    u16 year;

}RTC_DATE;

void RTC_Init(void);

void RTC_SetTime(RTC_TIME *t);
void RTC_GetTime(RTC_TIME *t);

void RTC_SetDate(RTC_DATE *d);
void RTC_GetDate(RTC_DATE *d);

void RTC_Edit(void);

#endif

