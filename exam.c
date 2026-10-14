/* exam.c */

#include "exam.h"
#include "rtc.h"
#include "sevenseg.h"
#include "led.h"
#include "buzzer.h"
#include "lcd.h"
#include "interrupt.h"
#include "delay.h"

#define SECONDS_PER_DAY   86400UL

extern volatile unsigned char Pause_Request;

volatile SYSTEM_STATE SystemState = SYSTEM_IDLE;

extern RTC_TIME CurrentTime;
RTC_TIME ExamStartTime;
RTC_TIME ExamEndTime;
RTC_TIME ExamScheduledTime;

volatile unsigned char ExamScheduleSet = 0;
volatile unsigned char ExamLogPending  = 0;

static unsigned char ExamHasStarted = 0;
static unsigned char ExamHasEnded   = 0;

/* Countdown is driven by RTC seconds-of-day, so time spent inside menus
 * (password, RTC edit, ...) is still counted and the end time stays exact. */
static unsigned long LastTotalSec = 0;
static unsigned long ElapsedSec   = 0;

unsigned int ExamDuration = 0;
unsigned int RemainingMinutes = 0;
unsigned int PauseCount = 0;

static unsigned long TimeToSeconds(const RTC_TIME *t)
{
    return ((unsigned long)t->hour * 3600UL) +
           ((unsigned long)t->min  * 60UL)   +
            (unsigned long)t->sec;
}

static void LCD_PrintTime(const RTC_TIME *t)
{
    if(t->hour < 10) LCD_Data('0');
    LCD_Integer(t->hour);
    LCD_Data(':');
    if(t->min < 10) LCD_Data('0');
    LCD_Integer(t->min);
    LCD_Data(':');
    if(t->sec < 10) LCD_Data('0');
    LCD_Integer(t->sec);
}

void Exam_Init(void)
{
    RemainingMinutes = 0;
    PauseCount       = 0;
    ElapsedSec       = 0;
    LastTotalSec     = 0;
    ExamHasStarted   = 0;
    ExamHasEnded     = 0;
    ExamScheduleSet  = 0;
    ExamLogPending   = 0;
}

void Exam_SetSchedule(u8 hour, u8 min)
{
    ExamScheduledTime.hour = hour;
    ExamScheduledTime.min  = min;
    ExamScheduledTime.sec  = 0;
    ExamScheduleSet        = 1;
}

/* Called from the main loop: starts the exam by itself when the RTC reaches
 * the configured start time (hour and minute match). */
u8 Exam_CheckSchedule(void)
{
    if(!ExamScheduleSet)
        return 0;

    if(SystemState == SYSTEM_RUNNING || SystemState == SYSTEM_PAUSED)
        return 0;

    if(ExamDuration == 0)
        return 0;

    RTC_GetTime(&CurrentTime);

    if(CurrentTime.hour == ExamScheduledTime.hour &&
       CurrentTime.min  == ExamScheduledTime.min)
    {
        StartExam();
        return (SystemState == SYSTEM_RUNNING);
    }

    return 0;
}

void StartExam(void)
{
    if(ExamDuration == 0)
        return;

    /* Record the exact start time from the RTC */
    RTC_GetTime(&ExamStartTime);

    /* Clear previous end time */
    ExamEndTime.hour = 0;
    ExamEndTime.min  = 0;
    ExamEndTime.sec  = 0;

    ExamHasStarted  = 1;
    ExamHasEnded    = 0;
    ExamScheduleSet = 0;      /* schedule is used up */

    RemainingMinutes = ExamDuration;
    PauseCount       = 0;      /* new exam, new pause count */
    ElapsedSec       = 0;
    LastTotalSec     = TimeToSeconds(&ExamStartTime);

    Pause_Request = 0;

    LED_Status(RemainingMinutes, ExamDuration);

    SevenSeg_Display(RemainingMinutes);

    SystemState = SYSTEM_RUNNING;
}

void PauseExam(void)
{
    PauseCount++;

    SystemState = SYSTEM_PAUSED;
}

void ResumeExam(void)
{
    /* Restart the reference point so paused time is not counted */
    RTC_GetTime(&CurrentTime);

    LastTotalSec = TimeToSeconds(&CurrentTime);

    SystemState = SYSTEM_RUNNING;
}

void StopExam(void)
{
    /* Record the exact end time from the RTC */
    RTC_GetTime(&ExamEndTime);

    ExamHasEnded   = 1;
    ExamLogPending = 1;

    RemainingMinutes = 0;

    SevenSeg_Display(0);

    LED_Status(0, ExamDuration);

    Buzzer_Beep(3000);

    SystemState = SYSTEM_FINISHED;
}

void Exam_Process(void)
{
    unsigned long now;
    unsigned long delta;
    unsigned int  newRemaining;

    SevenSeg_Display(RemainingMinutes);

    if(SystemState != SYSTEM_RUNNING)
        return;

    if(Pause_Request)
        return;

    RTC_GetTime(&CurrentTime);

    now = TimeToSeconds(&CurrentTime);

    if(now >= LastTotalSec)
        delta = now - LastTotalSec;
    else
        delta = (SECONDS_PER_DAY - LastTotalSec) + now;   /* midnight rollover */

    if(delta == 0)
        return;

    LastTotalSec = now;
    ElapsedSec  += delta;

    if(ElapsedSec >= ((unsigned long)ExamDuration * 60UL))
    {
        /* Stop immediately at 00 */
        StopExam();
        return;
    }

    newRemaining = ExamDuration - (unsigned int)(ElapsedSec / 60UL);

    if(newRemaining != RemainingMinutes)
    {
        RemainingMinutes = newRemaining;

        LED_Status(RemainingMinutes, ExamDuration);

        SevenSeg_Display(RemainingMinutes);
    }
}

void ShowExamLogs(void)
{
    LCD_Clear();

    if(!ExamHasStarted)
    {
        LCD_String("NO EXAM YET");
        delay_ms(1500);
        LCD_Clear();
        return;
    }

    LCD_String("Start ");
    LCD_PrintTime(&ExamStartTime);

    LCD_GotoXY(1,0);

    if(ExamHasEnded)
    {
        LCD_String("End   ");
        LCD_PrintTime(&ExamEndTime);
    }
    else
    {
        LCD_String("End   RUNNING");
    }

    delay_ms(4000);

    LCD_Clear();
}
