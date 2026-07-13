#include "led_controller.h"

#include <Arduino.h>

#include "constants.h"

namespace LedController {

void begin() {
  pinMode(PIN_LED_ALARM, OUTPUT);
  pinMode(PIN_LED_NORMAL, OUTPUT);
  showNormal();
}

void showAlarm() {
  digitalWrite(PIN_LED_ALARM, HIGH);
  digitalWrite(PIN_LED_NORMAL, LOW);
}

void showNormal() {
  digitalWrite(PIN_LED_ALARM, LOW);
  digitalWrite(PIN_LED_NORMAL, HIGH);
}

}  // namespace LedController
