/* exam.c */

#include "exam.h"
#include "rtc.h"
#include "sevenseg.h"
#include "led.h"
#include "buzzer.h"
#include "lcd.h"
#include "interrupt.h"
#include "delay.h"

extern volatile unsigned char Pause_Request;

volatile SYSTEM_STATE SystemState = SYSTEM_IDLE;

extern RTC_TIME CurrentTime;
RTC_TIME ExamStartTime;
RTC_TIME ExamEndTime;

static unsigned char LastSecond = 255;
static unsigned char SecondCount = 0;

unsigned int ExamDuration = 0;
unsigned int RemainingMinutes = 0;

void Exam_Init(void)
{
    RemainingMinutes = 0;
    LastSecond = 255;
    SecondCount = 0;

    /* Nothing is running yet - make sure LEDs are off and the
     * 7-segment display is blank rather than showing "00". */
    LED_Status(0, 0);
    SevenSeg_Off();
}

void StartExam(void)
{
    /* Do not start if exam duration is zero */
    if(ExamDuration == 0)
        return;

    RTC_GetTime(&ExamStartTime);

    RemainingMinutes = ExamDuration;

    RTC_GetTime(&CurrentTime);

    LastSecond = CurrentTime.sec;
    SecondCount = 0;

    /* Clear any stale pause flag left over from a switch press that
     * happened before this exam started (e.g. while idle), so the
     * new exam never begins already paused. */
    Pause_Request = 0;

    LED_Status(RemainingMinutes, ExamDuration);

    SevenSeg_On();
    SevenSeg_Display(RemainingMinutes);

    SystemState = SYSTEM_RUNNING;
}

void PauseExam(void)
{
    SystemState = SYSTEM_PAUSED;
}

void ResumeExam(void)
{
    RTC_GetTime(&CurrentTime);

    LastSecond = CurrentTime.sec;

    SystemState = SYSTEM_RUNNING;
}

void StopExam(void)
{
    RTC_GetTime(&ExamEndTime);

    RemainingMinutes = 0;

    SevenSeg_Display(0);
    SevenSeg_Off();

    LED_Status(0, ExamDuration);

    Buzzer_Beep(3000);

    SystemState = SYSTEM_FINISHED;
}

void Exam_Process(void)
{
    /* Keep refreshing display */
    SevenSeg_Display(RemainingMinutes);

    if(SystemState != SYSTEM_RUNNING)
        return;

    if(Pause_Request)
        return;

    RTC_GetTime(&CurrentTime);

    /* Execute only once every RTC second */
    if(CurrentTime.sec != LastSecond)
    {
        LastSecond = CurrentTime.sec;

        SecondCount++;

        if(SecondCount >= 60)
        {
            SecondCount = 0;

            if(RemainingMinutes > 0)
            {
                RemainingMinutes--;

                LED_Status(RemainingMinutes, ExamDuration);

                SevenSeg_Display(RemainingMinutes);

                /* Stop immediately at 00 */
                if(RemainingMinutes == 0)
                {
                    StopExam();
                }
            }
        }
    }
}

void ShowExamLogs(void)
{
    LCD_Clear();

    LCD_String("Start:");

    if(ExamStartTime.hour < 10)
        LCD_Data('0');
    LCD_Integer(ExamStartTime.hour);

    LCD_Data(':');

    if(ExamStartTime.min < 10)
        LCD_Data('0');
    LCD_Integer(ExamStartTime.min);

    LCD_Data(':');

    if(ExamStartTime.sec < 10)
        LCD_Data('0');
    LCD_Integer(ExamStartTime.sec);

    delay_ms(2000);

    LCD_Clear();

    LCD_String("End:");

    if(ExamEndTime.hour < 10)
        LCD_Data('0');
    LCD_Integer(ExamEndTime.hour);

    LCD_Data(':');

    if(ExamEndTime.min < 10)
        LCD_Data('0');
    LCD_Integer(ExamEndTime.min);

    LCD_Data(':');

    if(ExamEndTime.sec < 10)
        LCD_Data('0');
    LCD_Integer(ExamEndTime.sec);

    delay_ms(3000);

    LCD_Clear();
}
