# SMART EXAM HALL – Exam Monitoring System

Embedded exam-hall monitoring system built as a Keil uVision (ARM) project.

## Features
- Password-protected menu (keypad + LCD)
- Exam start time (auto-start when RTC matches) or start-now
- Countdown timer based on RTC, with pause count
- LED status: Green (>10 min), Yellow (last 10 min), Red (last 1 min)
- Buzzer alert and start/end time log
- LM35 temperature reading via ADC
- Seven-segment display output

## Build
Open `SMART_EXAM_monitoring sys.uvproj` in Keil uVision and do a full **Rebuild**.

## Notes
See `CHANGES.txt` for the latest changes.
