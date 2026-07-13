#ifndef BUZZER_CONTROLLER_H
#define BUZZER_CONTROLLER_H

// ============================================================
// buzzer_controller — drives the active buzzer.
// Beeps in a 500 ms on / 500 ms off pattern while fire is
// detected. An intermittent beep is more attention-grabbing
// (and less annoying to debug) than a continuous tone.
// ============================================================

// Configure the buzzer pin. Call once from setup().
void buzzerSetup();

// Beep pattern. Call every loop() pass while the alarm is active.
void buzzerBeep(unsigned long now);

// Force the buzzer silent (used when leaving the alarm state).
void buzzerOff();

#endif // BUZZER_CONTROLLER_H
