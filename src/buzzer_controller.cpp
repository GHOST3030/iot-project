#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "buzzer_controller.h"

static bool buzzerState = false;
static unsigned long lastToggleTime = 0;

void buzzerSetup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void buzzerBeep(unsigned long now) {
  // Toggle every BUZZER_BEEP_INTERVAL_MS without blocking the loop.
  if (now - lastToggleTime >= BUZZER_BEEP_INTERVAL_MS) {
    lastToggleTime = now;
    buzzerState = !buzzerState;
    digitalWrite(PIN_BUZZER, buzzerState ? HIGH : LOW);
  }
}

void buzzerOff() {
  buzzerState = false;
  digitalWrite(PIN_BUZZER, LOW);
}
