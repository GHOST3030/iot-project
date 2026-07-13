#include "relay_controller.h"

#include <Arduino.h>

#include "constants.h"

namespace RelayController {

static bool s_pumpOn = false;

void begin() {
  pinMode(PIN_RELAY, OUTPUT);
  setPump(false);
}

void setPump(bool on) {
  s_pumpOn = on;
  // Translate logical on/off into the module's electrical polarity.
  bool level = RELAY_ACTIVE_LOW ? !on : on;
  digitalWrite(PIN_RELAY, level ? HIGH : LOW);
}

bool isPumpOn() { return s_pumpOn; }

}  // namespace RelayController
