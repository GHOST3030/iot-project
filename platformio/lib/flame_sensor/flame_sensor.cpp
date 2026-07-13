#include "flame_sensor.h"

#include <Arduino.h>

#include "constants.h"

namespace FlameSensor {

void begin() {
  // INPUT_PULLUP lets a simple pushbutton simulate the sensor in Wokwi.
  // A real flame module drives the pin, so the pull-up is harmless.
  pinMode(PIN_FLAME_SENSOR, INPUT_PULLUP);
}

bool isFlameDetected() {
  int reading = digitalRead(PIN_FLAME_SENSOR);
  return FLAME_ACTIVE_LOW ? (reading == LOW) : (reading == HIGH);
}

}  // namespace FlameSensor
