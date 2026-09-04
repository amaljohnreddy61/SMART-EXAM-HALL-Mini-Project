/* main.c */
#include <lpc214x.h>
#include "keypad.h"
#include "lcd.h"
#include "rtc.h"
#include "adc.h"
#include "delay.h"
#include "password.h"
#include "interrupt.h"
#include "exam.h"
#include "led.h"
#include "buzzer.h"
#include "sevenseg.h"

RTC_TIME CurrentTime;
RTC_DATE CurrentDate;
float Temperature;

extern unsigned int ExamDuration;
extern unsigned int RemainingMinutes;

static void DisplayHomeScreen(void);
static void AdminMenu(void);
static void ConfigureExamTime(void);

int main(void)
{
    LCD_Init();
    RTC_Init();
    ADC_Init();
    LED_Init();
    Buzzer_Init();
    SevenSeg_Init();
    Keypad_Init();
    Exam_Init();
    Interrupt_Init();

    LCD_String("WELCOME");
    delay_ms(1000);
    LCD_Clear();
    LCD_String("SMART EXAM HALL");
    delay_ms(1000);

    while(1)
    {
       RTC_GetTime(&CurrentTime);
				RTC_GetDate(&CurrentDate);
				Temperature = GetTemperature();

			/* Refresh 7-segment display */
				SevenSeg_Display(RemainingMinutes);

				DisplayHomeScreen();
        if(Edit_Request)
        {
            if(VerifyPassword())
            {
                AdminMenu();
            }
            Edit_Request = 0;
        }

        if(SystemState == SYSTEM_RUNNING)
{
    Exam_Process();

    if(Pause_Request)
    {
        PauseExam();

        LCD_Clear();
        LCD_String("EXAM PAUSED");

        while(Pause_Request)
        {
            SevenSeg_Display(RemainingMinutes);
        }

        ResumeExam();

        LCD_Clear();
        LCD_String("EXAM RESUMED");
        delay_ms(1000);
        LCD_Clear();
    }
}			
    }
}

static void DisplayHomeScreen(void)
{
    /* First Line : Time and Temperature */

    LCD_GotoXY(0,0);

    if(CurrentTime.hour < 10)
        LCD_Data('0');
    LCD_Integer(CurrentTime.hour);

    LCD_Data(':');

    if(CurrentTime.min < 10)
        LCD_Data('0');
    LCD_Integer(CurrentTime.min);

    LCD_Data(':');

    if(CurrentTime.sec < 10)
        LCD_Data('0');
    LCD_Integer(CurrentTime.sec);

    LCD_String(" ");

    if((int)Temperature < 10)
        LCD_Data('0');
    LCD_Integer((int)Temperature);

    LCD_String(" C   ");

    /* Second Line : Date. Remaining exam time is shown on the
     * dedicated 7-segment display instead, so the LCD's second
     * line just keeps showing the date at all times. */

    LCD_GotoXY(1,0);

    if(CurrentDate.date < 10)
        LCD_Data('0');
    LCD_Integer(CurrentDate.date);

    LCD_Data('/');

    if(CurrentDate.month < 10)
        LCD_Data('0');
    LCD_Integer(CurrentDate.month);

    LCD_Data('/');

    LCD_Integer(CurrentDate.year);

    LCD_String("     ");
}

static void AdminMenu(void)
{
    char key;
    while(1)
    {
        LCD_Clear();
        LCD_String("1RTC 2TIME");
        LCD_GotoXY(1,0);
        LCD_String("3ST 4LOG #EX");
        key = Keypad_GetKey();
        switch(key)
        {
            case '1':
                if(SystemState == SYSTEM_RUNNING || SystemState == SYSTEM_PAUSED)
                {
                    LCD_Clear();
                    LCD_String("EXAM IN PROGRESS");
                    delay_ms(1000);
                }
                else
                {
                    RTC_Edit();
                }
                break;
            case '2':
                if(SystemState == SYSTEM_RUNNING || SystemState == SYSTEM_PAUSED)
                {
                    LCD_Clear();
                    LCD_String("EXAM IN PROGRESS");
                    delay_ms(1000);
                }
                else
                {
                    ConfigureExamTime();
                }
                break;
            case '3':

						if(SystemState == SYSTEM_RUNNING || SystemState == SYSTEM_PAUSED)
							{
								/* Don't let a second EINT0 press reset an
								 * exam that is already in progress. */
								LCD_Clear();
								LCD_String("EXAM IN PROGRESS");
								delay_ms(1000);
							}
						else if(ExamDuration == 0)
							{
								LCD_Clear();
								LCD_String("SET TIME");
								delay_ms(1000);
							}
						else
						{
						StartExam();

						LCD_Clear();
						LCD_String("EXAM STARTED");

						delay_ms(1000);

					return;
					}
					break;
            case '4':
                ShowExamLogs();
                break;
            case '#':
                return;
        }
    }
}

static void ConfigureExamTime(void)
{
    char d1,d2;

    LCD_Clear();

    /* Duration is documented/displayed as 00-99 minutes (2 digits,
     * matching the 7-segment display) - previously this read a 3rd
     * digit which allowed entry up to 999 before silently clamping
     * to 99, which didn't match the stated range or give feedback. */
    LCD_String("Minutes(00-99):");

    LCD_GotoXY(1,0);

    /* Non-digit keys (A/B/C/D/*/#) are rejected and re-prompted for,
     * same as RTC_Edit()'s digit entry, instead of silently falling
     * through to garbage that only the >99 clamp below happens to
     * catch. */
    do { d1 = Keypad_GetKey(); } while(d1 < '0' || d1 > '9');
    LCD_Data(d1);

    do { d2 = Keypad_GetKey(); } while(d2 < '0' || d2 > '9');
    LCD_Data(d2);

    ExamDuration =
        (d1 - '0') * 10 +
        (d2 - '0');

    if(ExamDuration > 99)
        ExamDuration = 99;

    RemainingMinutes = ExamDuration;

			LCD_Clear();
			LCD_String("Saved");

				delay_ms(1000);

				LCD_Clear();
}
