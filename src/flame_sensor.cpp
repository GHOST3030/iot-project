#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "flame_sensor.h"

// Debounced, confirmed state exposed to the rest of the system.
static bool fireDetected = false;

// Internal debounce bookkeeping.
static bool lastRawReading = false;
static uint8_t stableCount = 0;
static unsigned long lastPollTime = 0;

void flameSensorSetup() {
  // INPUT_PULLUP keeps the pin HIGH (no fire) when nothing pulls it
  // down. The real sensor module actively drives the pin, and in
  // Wokwi the slide switch connects it to GND — both work with this.
  pinMode(PIN_FLAME_SENSOR, INPUT_PULLUP);
}

void flameSensorUpdate(unsigned long now) {
  // Non-blocking rate limit: only sample every SENSOR_POLL_INTERVAL_MS.
  if (now - lastPollTime < SENSOR_POLL_INTERVAL_MS) {
    return;
  }
  lastPollTime = now;

  bool rawFire = (digitalRead(PIN_FLAME_SENSOR) == FLAME_ACTIVE_LEVEL);

  if (rawFire == lastRawReading) {
    // Reading is holding steady — count it (saturate to avoid overflow).
    if (stableCount < FLAME_DEBOUNCE_COUNT) {
      stableCount++;
    }
  } else {
    // Reading changed — restart the stability count.
    lastRawReading = rawFire;
    stableCount = 1;
  }

  // Only accept the new state after it has proven stable.
  if (stableCount >= FLAME_DEBOUNCE_COUNT) {
    fireDetected = rawFire;
  }
}

bool flameSensorFireDetected() {
  return fireDetected;
}
