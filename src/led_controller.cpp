#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "led_controller.h"

static bool ledState = false;
static unsigned long lastToggleTime = 0;

void ledSetup() {
  pinMode(PIN_ALARM_LED, OUTPUT);
  digitalWrite(PIN_ALARM_LED, LOW);
}

void ledBlink(unsigned long now) {
  // Toggle every LED_BLINK_INTERVAL_MS without blocking the loop.
  if (now - lastToggleTime >= LED_BLINK_INTERVAL_MS) {
    lastToggleTime = now;
    ledState = !ledState;
    digitalWrite(PIN_ALARM_LED, ledState ? HIGH : LOW);
  }
}

void ledOff() {
  ledState = false;
  digitalWrite(PIN_ALARM_LED, LOW);
}
