/*
 * main.cpp — Smart Fire Detection & Water Extinguishing System (ESP32)
 * --------------------------------------------------------------------
 * The orchestrator. It owns the fire STATE MACHINE and coordinates the
 * hardware/connectivity modules; each module keeps its own responsibility.
 *
 *   Modules:  flame_sensor · led_controller · buzzer_controller ·
 *             relay_controller · wifi_manager · blynk_manager ·
 *             config · constants
 *
 * State machine:
 *   NORMAL  --flame held FLAME_CONFIRM_MS-->  FIRE
 *   FIRE    --no flame for FLAME_CLEAR_MS-->  COOLDOWN   (safety spray window)
 *   COOLDOWN--flame again-->  FIRE
 *   COOLDOWN--calm for 2*FLAME_CLEAR_MS-->  NORMAL
 *
 * Design rule: the fire logic is fully local. Wi-Fi/Blynk add remote
 * monitoring and manual control, but the system detects and extinguishes
 * even with no network. Uses millis() throughout — never delay().
 */

#include <Arduino.h>

#include "blynk_manager.h"
#include "buzzer_controller.h"
#include "config.h"
#include "constants.h"
#include "flame_sensor.h"
#include "led_controller.h"
#include "relay_controller.h"
#include "wifi_manager.h"

// ===================== State =====================
static SystemState g_state = SystemState::NORMAL;
static bool g_fireNotified = false;

static unsigned long g_flameFirstSeenAt = 0;
static unsigned long g_flameLastSeenAt = 0;
static unsigned long g_lastSensorPoll = 0;
static unsigned long g_lastStatusPublish = 0;

// ===================== Helpers =====================
const char* stateName(SystemState s) {
  switch (s) {
    case SystemState::FIRE:     return "FIRE";
    case SystemState::COOLDOWN: return "COOLDOWN";
    default:                    return "NORMAL";
  }
}

// Drive every output for a given "fire active" decision.
static void applyOutputs(bool fireActive) {
  if (fireActive) {
    LedController::showAlarm();
    BuzzerController::on();
    RelayController::setPump(true);
  } else {
    LedController::showNormal();
    BuzzerController::off();
    RelayController::setPump(false);
  }
}

// ===================== State Machine =====================
static void updateStateMachine(unsigned long now, bool flame) {
  // Track flame timing for debounce and clear windows.
  if (flame) {
    g_flameLastSeenAt = now;
    if (g_flameFirstSeenAt == 0) g_flameFirstSeenAt = now;
  } else {
    g_flameFirstSeenAt = 0;
  }

  switch (g_state) {
    case SystemState::NORMAL:
      // Require a sustained flame before alarming (debounce).
      if (flame && now - g_flameFirstSeenAt >= FLAME_CONFIRM_MS) {
        g_state = SystemState::FIRE;
        applyOutputs(true);
        Serial.println(">>> FIRE DETECTED — alarm + pump ON");
        if (!g_fireNotified) {
          BlynkManager::notifyFire();
          g_fireNotified = true;
        }
      }
      break;

    case SystemState::FIRE:
      // Keep spraying through a safety window after the flame is gone.
      if (!flame && now - g_flameLastSeenAt >= FLAME_CLEAR_MS) {
        g_state = SystemState::COOLDOWN;
        applyOutputs(false);
        Serial.println("Flame cleared — pump OFF, cooling down");
      }
      break;

    case SystemState::COOLDOWN:
      if (flame) {
        g_state = SystemState::FIRE;
        applyOutputs(true);
        Serial.println("Re-ignition! alarm + pump back ON");
      } else if (now - g_flameLastSeenAt >= 2 * FLAME_CLEAR_MS) {
        g_state = SystemState::NORMAL;
        g_fireNotified = false;
        Serial.println("System back to NORMAL");
      }
      break;
  }
}

// ===================== Dashboard =====================
static void publishStatus(unsigned long now) {
  if (now - g_lastStatusPublish < STATUS_PUBLISH_MS) return;
  g_lastStatusPublish = now;

  bool fireActive = (g_state == SystemState::FIRE);
  Serial.printf("[%s] flame=%s pump=%s wifi=%s blynk=%s\n",
                stateName(g_state),
                FlameSensor::isFlameDetected() ? "YES" : "no",
                RelayController::isPumpOn() ? "ON" : "OFF",
                WifiManager::isConnected() ? "up" : "down",
                BlynkManager::isConnected() ? "up" : "down");

  BlynkManager::publishStatus(fireActive, RelayController::isPumpOn());
}

// ===================== Setup =====================
void setup() {
  Serial.begin(SERIAL_BAUD);
  delay(200);  // let the USB serial settle (one-time, not in the loop)
  Serial.println("\n=== Smart Fire Detection System — starting ===");

  // Bring up every module (each sets its own safe initial state).
  FlameSensor::begin();
  LedController::begin();
  BuzzerController::begin();
  RelayController::begin();
  WifiManager::begin();
  BlynkManager::begin();

  Serial.println("All modules initialized. Monitoring for fire...");
}

// ===================== Loop =====================
void loop() {
  unsigned long now = millis();

  // Connectivity (non-blocking).
  WifiManager::loop();
  BlynkManager::loop();

  // Sample the sensor and run the state machine on a fixed cadence.
  if (now - g_lastSensorPoll >= SENSOR_POLL_MS) {
    g_lastSensorPoll = now;
    bool flame = FlameSensor::isFlameDetected();
    updateStateMachine(now, flame);

    // Manual override from the app forces the pump ON regardless of state.
    if (BlynkManager::manualPumpOverride() && !RelayController::isPumpOn()) {
      RelayController::setPump(true);
      Serial.println("Manual override — pump ON");
    }
  }

  publishStatus(now);
}
