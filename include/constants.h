#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <Arduino.h>

// ============================================================
// constants.h — Fixed values used across the firmware.
// No magic numbers anywhere else: if a number matters, it is
// named here and explained.
// Unlike config.h, you normally do NOT change this file when
// moving to real hardware.
// ============================================================

// ------------------------------------------------------------
// System states (the heart of the firmware)
// ------------------------------------------------------------
// The system is a small state machine:
//   STATE_IDLE        — monitoring, all outputs off
//   STATE_FIRE_ALARM  — fire detected: LED + buzzer + pump on
//   STATE_MANUAL_PUMP — no fire, but user forced the pump on
//                       from the Blynk dashboard
enum SystemState {
  STATE_IDLE,
  STATE_FIRE_ALARM,
  STATE_MANUAL_PUMP
};

// ------------------------------------------------------------
// Signal polarity
// ------------------------------------------------------------
// Flame sensor modules pull their digital output LOW on flame
// (active-low). Kept as a constant so a different sensor only
// needs one change.
const int FLAME_ACTIVE_LEVEL = LOW;

// Most hobby relay boards are active-HIGH on the IN pin; some
// are active-LOW. Flip this single constant if your board is.
const int RELAY_ACTIVE_LEVEL = HIGH;

// ------------------------------------------------------------
// Timing (all in milliseconds, used with millis(), never delay())
// ------------------------------------------------------------
// How often we sample the flame sensor.
const unsigned long SENSOR_POLL_INTERVAL_MS = 50;

// The sensor reading must stay stable this many consecutive polls
// before we accept it (debounce: 3 x 50 ms = 150 ms). This filters
// electrical noise and prevents false alarms.
const uint8_t FLAME_DEBOUNCE_COUNT = 3;

// Alarm LED blink period while fire is detected (on 250 ms / off 250 ms).
const unsigned long LED_BLINK_INTERVAL_MS = 250;

// Buzzer beep pattern while fire is detected (on 500 ms / off 500 ms).
const unsigned long BUZZER_BEEP_INTERVAL_MS = 500;

// Give up on the initial Wi-Fi connection after this long and keep
// running offline (fire detection must never depend on the cloud!).
const unsigned long WIFI_CONNECT_TIMEOUT_MS = 15000;

// Progress dot printing while connecting to Wi-Fi.
const unsigned long WIFI_PROGRESS_INTERVAL_MS = 500;

// How often we push status (fire/pump/uptime) to the Blynk dashboard.
const unsigned long TELEMETRY_INTERVAL_MS = 1000;

// If the Blynk connection drops, retry at most this often.
const unsigned long BLYNK_RECONNECT_INTERVAL_MS = 10000;

// ------------------------------------------------------------
// Serial
// ------------------------------------------------------------
const unsigned long SERIAL_BAUD_RATE = 115200;

// ------------------------------------------------------------
// Blynk virtual pins (dashboard datastreams)
// ------------------------------------------------------------
// V0 — Fire Status   (read-only LED widget)
// V1 — Pump Status   (read-only LED widget)
// V2 — Manual Pump   (switch widget, writes to device)
// V3 — Uptime seconds (proves the device is online)
const uint8_t VPIN_FIRE_STATUS = 0;
const uint8_t VPIN_PUMP_STATUS = 1;
const uint8_t VPIN_MANUAL_PUMP = 2;
const uint8_t VPIN_UPTIME = 3;

#endif // CONSTANTS_H
