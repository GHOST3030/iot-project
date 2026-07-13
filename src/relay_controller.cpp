#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "relay_controller.h"

static bool pumpOn = false;

void relaySetup() {
  pinMode(PIN_RELAY, OUTPUT);
  // Start with the relay released so the pump is guaranteed off at boot.
  digitalWrite(PIN_RELAY, RELAY_ACTIVE_LEVEL == HIGH ? LOW : HIGH);
}

void relaySetPump(bool on) {
  // Only touch the hardware (and the log) when the state actually changes.
  if (on == pumpOn) {
    return;
  }
  pumpOn = on;

  if (RELAY_ACTIVE_LEVEL == HIGH) {
    digitalWrite(PIN_RELAY, on ? HIGH : LOW);
  } else {
    digitalWrite(PIN_RELAY, on ? LOW : HIGH);
  }

  Serial.print("[RELAY] Water pump: ");
  Serial.println(on ? "ON" : "OFF");
}

bool relayIsPumpOn() {
  return pumpOn;
}
