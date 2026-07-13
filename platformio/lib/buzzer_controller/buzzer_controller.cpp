#include "buzzer_controller.h"

#include <Arduino.h>

#include "constants.h"

namespace BuzzerController {

void begin() {
  pinMode(PIN_BUZZER, OUTPUT);
  off();
}

void on() { digitalWrite(PIN_BUZZER, HIGH); }

void off() { digitalWrite(PIN_BUZZER, LOW); }

}  // namespace BuzzerController
