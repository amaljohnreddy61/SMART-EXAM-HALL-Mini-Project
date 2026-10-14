#ifndef __EXAM_H__
#define __EXAM_H__

#include "rtc.h"


typedef enum
{
    SYSTEM_IDLE = 0,
    SYSTEM_PASSWORD,   /* Reserved: password entry currently runs as a direct
                         * call to VerifyPassword() from main.c rather than
                         * through SystemState. */
    SYSTEM_MENU,       /* Reserved: the admin menu currently runs as a direct
                         * call to AdminMenu() from main.c rather than
                         * through SystemState. */
    SYSTEM_RUNNING,
    SYSTEM_PAUSED,
    SYSTEM_FINISHED

} SYSTEM_STATE;

extern volatile SYSTEM_STATE SystemState;

extern RTC_TIME ExamStartTime;       /* RTC time recorded when exam started  */
extern RTC_TIME ExamEndTime;         /* RTC time recorded when exam ended    */
extern RTC_TIME ExamScheduledTime;   /* start time configured by invigilator */

extern volatile unsigned char ExamScheduleSet;   /* 1 = scheduled start armed   */
extern volatile unsigned char ExamLogPending;    /* 1 = show log after exam end */

extern unsigned int ExamDuration;
extern unsigned int RemainingMinutes;
extern unsigned int PauseCount;      /* times the exam was paused (this exam) */

void Exam_Init(void);
void Exam_SetSchedule(u8 hour, u8 min);
u8   Exam_CheckSchedule(void);       /* returns 1 if it auto-started the exam */
void StartExam(void);
void PauseExam(void);
void ResumeExam(void);
void StopExam(void);
void Exam_Process(void);
void ShowExamLogs(void);
#endif
