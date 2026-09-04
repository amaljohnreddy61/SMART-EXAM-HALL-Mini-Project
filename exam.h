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

extern RTC_TIME ExamStartTime;
extern RTC_TIME ExamEndTime;

extern unsigned int ExamDuration;
extern unsigned int RemainingMinutes;

void Exam_Init(void);
void StartExam(void);
void PauseExam(void);
void ResumeExam(void);
void StopExam(void);
void Exam_Process(void);
void ShowExamLogs(void);
#endif
