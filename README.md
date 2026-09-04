# Smart Exam Hall Monitoring and Management System

An LPC2148 (ARM7TDMI-S) embedded mini-project that manages exam sessions
in a hall: it keeps time via an onboard RTC, shows a live countdown on a
7-segment display, gives a traffic-light-style LED status (green / yellow /
red) based on how much time is left, monitors room temperature, and logs
exam start/end times — all controlled through a keypad + password-protected
admin menu.

## Features

- **RTC-based clock** — time/date shown on a 16x2 LCD, persists across
  power cycles using the battery-backed `GPREG0` register (no re-stamping
  the default date every reset).
- **7-segment countdown timer** — shows remaining exam minutes. Refreshed
  by a Timer0 interrupt (~250Hz per digit) so it stays flicker-free
  regardless of what the LCD/keypad code is doing. Blank when idle, lights
  up when an exam starts.
- **LED exam-status indicator** — GREEN / YELLOW / RED, with thresholds
  scaled as a percentage of the *configured* exam duration (not a fixed
  number of minutes), so it behaves sensibly whether the exam is 5 minutes
  or 90. The final minute is always RED. (Note: exams of 2–3 minutes may
  not show YELLOW at all — not enough discrete minutes to fit all three
  states; see comments in `led.c`.)
- **Temperature monitoring** — LM35 sensor read via the onboard ADC,
  shown on the LCD alongside the clock.
- **Keypad + password-protected admin menu** — set the RTC, configure
  exam duration (00–99 minutes), start/stop an exam, view start/end time
  logs.
- **Pause / edit hardware buttons** — EINT0/EINT1 interrupts for a
  quick pause toggle and an edit-request, debounced in software.
- **Buzzer alert** at exam end.

## Hardware

- Microcontroller: **NXP LPC2148** (ARM7TDMI-S)
- 16x2 character LCD (parallel, 8-bit mode)
- 2-digit multiplexed 7-segment display (common-anode, active-low segments)
- 3x status LEDs (green / yellow / red)
- 4x4 (or similar) matrix keypad
- LM35 temperature sensor on an ADC input
- Buzzer
- 2x push buttons for pause/edit (EINT0 on P0.1, EINT1 on P0.3)

### Pin summary

| Peripheral        | Pins                                   |
|--------------------|-----------------------------------------|
| LCD data (D0–D7)  | P0.4–P0.11                              |
| LCD RS / EN       | P0.12 / P0.13                           |
| 7-seg DIGIT1/DIGIT2| P0.0 / P0.15                           |
| 7-seg segments A–G, DP | P0.16–P0.23                        |
| LEDs (G/Y/R)      | P1.24 / P1.25 / P1.26                   |
| Keypad rows/cols   | P1.16–P1.23                             |
| Temperature (LM35) | P0.30 (AD0.3)                          |
| Buzzer             | P0.27                                  |
| Pause / Edit buttons | P0.1 (EINT0) / P0.3 (EINT1)          |

> DIGIT1 is intentionally on P0.0 rather than P0.14 — P0.14 is the
> LPC2148's ISP/boot-select pin, and most dev boards have an external
> pull-up on it for the ISP button that fights a GPIO trying to drive it.

## Building

Requires **Keil µVision** (uVision4/5) with an ARM7/LPC2100 device
support pack installed.

1. Open `SMART_EXAM_monitoring_sys.uvproj` in Keil µVision.
2. Build → Rebuild All.
3. Flash the resulting `.hex`/`.axf` to your LPC2148 board (Flash Magic,
   ISP, or your programmer of choice).

## Project structure

All source lives at the repo root (matching the flat layout the `.uvproj`
expects):

```
main.c / main.c        - system init, main loop, LCD home screen, admin menu
exam.c / exam.h         - exam state machine (start/pause/resume/stop, logs)
rtc.c / rtc.h           - RTC init/read, GPREG0-based persistence
lcd.c / lcd.h           - 16x2 LCD driver
sevenseg.c / sevenseg.h - Timer0-interrupt-driven 7-segment display
led.c / led.h           - GREEN/YELLOW/RED status LEDs
keypad.c / keypad.h     - matrix keypad scanning
password.c / password.h- admin password entry/verification
adc.c / adc.h           - ADC / LM35 temperature reading
buzzer.c / buzzer.h     - buzzer control
interrupt.c / interrupt.h - EINT0/EINT1 pause & edit buttons
delay.c / delay.h       - busy-wait delay helpers
types.h / defines.h     - shared typedefs and constants
Startup.s               - Keil ARM startup/vector table
```

## Known limitations

- Exams of 2–3 minutes' duration may skip the YELLOW LED state entirely
  (integer-minute granularity leaves no room for it at that length).
- LCD temperature accuracy depends on the LM35 being wired to the ADC
  channel/pin the firmware expects (P0.30 / AD0.3) and the assumed 3.3V
  ADC reference matching your board.

## Recent fixes

- Admin menu options **1 (Edit RTC)** and **2 (Configure Exam Time)** are
  now blocked while an exam is `RUNNING`/`PAUSED` (shows "EXAM IN
  PROGRESS"), matching the guard option 3 already had. Previously,
  reconfiguring the duration mid-exam silently reset `RemainingMinutes`
  and desynced the LED/7-segment state.
- `ConfigureExamTime()` now validates keypad digits the same way
  `RTC_Edit()` does (rejects non-digit keys) instead of relying on the
  `>99` clamp as incidental protection.
