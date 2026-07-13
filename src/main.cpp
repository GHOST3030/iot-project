// ============================================================
// Smart Fire Detection & Water Extinguishing System — MVP
// Target: ESP32 DevKit V1 (Wokwi simulation & real hardware)
//
// main.cpp deliberately contains NO hardware details.
// It only wires the modules together and runs the state machine:
//
//   [flame_sensor] --> [state machine] --> [led / buzzer / relay]
//                            ^                     |
//                            |                     v
//                      [blynk_manager] <---- [wifi_manager]
//
// Priority rule: FIRE always wins. The manual pump switch can
// turn the pump on, but nothing can suppress a fire alarm.
// ============================================================

#include <Arduino.h>
#include "config.h"
#include "constants.h"
#include "flame_sensor.h"
#include "led_controller.h"
#include "buzzer_controller.h"
#include "relay_controller.h"
#include "wifi_manager.h"
#include "blynk_manager.h"

static SystemState currentState = STATE_IDLE;

// Human-readable state names for serial logging.
static const char* stateName(SystemState state) {
  switch (state) {
    case STATE_IDLE:        return "IDLE";
    case STATE_FIRE_ALARM:  return "FIRE_ALARM";
    case STATE_MANUAL_PUMP: return "MANUAL_PUMP";
    default:                return "UNKNOWN";
  }
}

// Decide what state the system SHOULD be in right now.
// Order encodes priority: fire beats everything.
static SystemState computeNextState() {
  if (flameSensorFireDetected()) {
    return STATE_FIRE_ALARM;
  }
  if (blynkManualPumpRequested()) {
    return STATE_MANUAL_PUMP;
  }
  return STATE_IDLE;
}

// Apply the transition (if any) and log it.
static void updateStateMachine() {
  SystemState nextState = computeNextState();
  if (nextState == currentState) {
    return;
  }

  Serial.print("[STATE] ");
  Serial.print(stateName(currentState));
  Serial.print(" -> ");
  Serial.println(stateName(nextState));

  if (nextState == STATE_FIRE_ALARM) {
    Serial.println("[ALARM] *** FIRE DETECTED - extinguishing! ***");
  }
  if (currentState == STATE_FIRE_ALARM) {
    Serial.println("[ALARM] Fire cleared.");
  }

  currentState = nextState;
}

// Drive every output according to the current state.
// Outputs are a pure function of state — easy to reason about.
static void applyOutputs(unsigned long now) {
  switch (currentState) {
    case STATE_FIRE_ALARM:
      ledBlink(now);
      buzzerBeep(now);
      relaySetPump(true);
      break;

    case STATE_MANUAL_PUMP:
      // Pump only: no siren for a user-requested test run.
      ledOff();
      buzzerOff();
      relaySetPump(true);
      break;

    case STATE_IDLE:
    default:
      ledOff();
      buzzerOff();
      relaySetPump(false);
      break;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println();
  Serial.println("=============================================");
  Serial.println(" Smart Fire Detection System - MVP booting");
  Serial.println("=============================================");

  // 1. Local hardware first — the safety core works without any network.
  flameSensorSetup();
  ledSetup();
  buzzerSetup();
  relaySetup();

  // 2. Connectivity second — optional, may fail without harming safety.
  wifiConnect();
  blynkSetup();

  Serial.println("[SYSTEM] Monitoring for fire...");
}

void loop() {
  // One timestamp per pass keeps all modules on the same clock.
  unsigned long now = millis();

  flameSensorUpdate(now);                                  // 1. sense
  updateStateMachine();                                    // 2. decide
  applyOutputs(now);                                       // 3. act
  blynkUpdate(flameSensorFireDetected(), relayIsPumpOn(), now); // 4. report
}
