#ifndef LED_CONTROLLER_H
#define LED_CONTROLLER_H

// ============================================================
// led_controller — drives the red alarm LED.
// Blinks (millis()-based, no delay()) while fire is detected.
// ============================================================

// Configure the LED pin. Call once from setup().
void ledSetup();

// Blink the LED. Call every loop() pass while the alarm is active.
void ledBlink(unsigned long now);

// Force the LED off (used when leaving the alarm state).
void ledOff();

#endif // LED_CONTROLLER_H
