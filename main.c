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
static void ConfigureStartTime(void);
static u8   ReadValue(const char *prompt, u8 minVal, u8 maxVal);
static u8   ExamInProgress(void);
static void ShowMessage(const char *msg);

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
    LCD_Clear();

    while(1)
    {
        RTC_GetTime(&CurrentTime);
        RTC_GetDate(&CurrentDate);
        Temperature = GetTemperature();

        if(SystemState >= SYSTEM_RUNNING)
        {
            SevenSeg_Display(RemainingMinutes);
        }

        DisplayHomeScreen();

        if(Edit_Request)
        {
            if(VerifyPassword())
            {
                AdminMenu();
            }
            Edit_Request = 0;
        }

        /* Scheduled start: exam begins automatically at the configured time */
        if(Exam_CheckSchedule())
        {
            ShowMessage("EXAM STARTED");
        }

        if(SystemState == SYSTEM_RUNNING)
        {
            Exam_Process();

            if(Pause_Request)
            {
                PauseExam();

                LCD_Clear();
                LCD_String("EXAM PAUSED");
                LCD_GotoXY(1,0);
                LCD_String("Pause count: ");
                if(PauseCount < 10)
                    LCD_Data('0');
                LCD_Integer(PauseCount);

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

        /* Exam just ended: show the recorded start and end times */
        if(ExamLogPending)
        {
            ExamLogPending = 0;
            ShowExamLogs();
        }
    }
}

static void DisplayHomeScreen(void)
{
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

    LCD_String("C");
    LCD_String("    ");

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

    if(SystemState >= SYSTEM_RUNNING)
    {
        /* After the exam has started: show how many times it was paused */
        LCD_String(" P:");

        if(PauseCount < 10)
            LCD_Data('0');
        LCD_Integer(PauseCount);

        LCD_String(" ");
    }
    else
    {
        LCD_String("     ");   /* before the exam: unchanged */
    }
}

static u8 ExamInProgress(void)
{
    return (SystemState == SYSTEM_RUNNING || SystemState == SYSTEM_PAUSED);
}

static void ShowMessage(const char *msg)
{
    LCD_Clear();
    LCD_String(msg);
    delay_ms(1000);
    LCD_Clear();
}

static void AdminMenu(void)
{
    char key;

    while(1)
    {
        LCD_Clear();
        LCD_String("1RTC 2DUR 3STIME");
        LCD_GotoXY(1,0);
        LCD_String("4START 5LOG #EX");

        key = Keypad_GetKey();

        switch(key)
        {
            case '1':   /* edit RTC date and time */
                if(ExamInProgress())
                    ShowMessage("EXAM IN PROGRESS");
                else
                    RTC_Edit();
                break;

            case '2':   /* exam duration */
                if(ExamInProgress())
                    ShowMessage("EXAM IN PROGRESS");
                else
                    ConfigureExamTime();
                break;

            case '3':   /* exam start time */
                if(ExamInProgress())
                    ShowMessage("EXAM IN PROGRESS");
                else
                    ConfigureStartTime();
                break;

            case '4':   /* start exam now */
                if(ExamInProgress())
                {
                    ShowMessage("EXAM IN PROGRESS");
                }
                else if(ExamDuration == 0)
                {
                    ShowMessage("SET DURATION");
                }
                else
                {
                    StartExam();
                    ShowMessage("EXAM STARTED");
                    return;
                }
                break;

            case '5':   /* start / end time log */
                ShowExamLogs();
                break;

            case '#':
                return;
        }
    }
}

/* Reads a two digit number from the keypad (non-digit keys are ignored)
 * and repeats the prompt until the value is inside minVal..maxVal. */
static u8 ReadValue(const char *prompt, u8 minVal, u8 maxVal)
{
    char d1, d2;
    u8 value;

    while(1)
    {
        LCD_Clear();
        LCD_String(prompt);
        LCD_GotoXY(1,0);

        do { d1 = Keypad_GetKey(); } while(d1 < '0' || d1 > '9');
        LCD_Data(d1);

        do { d2 = Keypad_GetKey(); } while(d2 < '0' || d2 > '9');
        LCD_Data(d2);

        value = (u8)((d1 - '0') * 10 + (d2 - '0'));

        if(value >= minVal && value <= maxVal)
            return value;

        ShowMessage("Invalid Value");
    }
}

static void ConfigureExamTime(void)
{
    ExamDuration = ReadValue("Minutes(01-99):", 1, 99);

    RemainingMinutes = ExamDuration;

    SevenSeg_Display((u8)ExamDuration);   /* 7-segment ON as soon as time is set */

    ShowMessage("Saved");
}

static void ConfigureStartTime(void)
{
    u8 hour, min;

    hour = ReadValue("Start Hour(0-23)", 0, 23);
    min  = ReadValue("Start Min(0-59):", 0, 59);

    Exam_SetSchedule(hour, min);

    ShowMessage("START TIME SAVED");

    if(ExamDuration == 0)
    {
        ShowMessage("NOW SET DURATION");
    }
}
