#ifndef CONSTANTS_H
#define CONSTANTS_H

/*
 * constants.h — single source of truth for hardware pins, timing,
 * polarity, and shared enums. No magic numbers anywhere else.
 *
 * To move from simulation to real hardware, in most cases you only
 * change the GPIO numbers below.
 */

// ===================== GPIO Pin Mapping =====================
constexpr int PIN_FLAME_SENSOR = 34;  // Flame sensor DO (input-only pin)
constexpr int PIN_RELAY        = 26;  // Relay IN → water pump
constexpr int PIN_BUZZER       = 27;  // Active buzzer
constexpr int PIN_LED_ALARM    = 25;  // Red LED  → fire alarm
constexpr int PIN_LED_NORMAL   = 33;  // Green LED → system healthy

// ===================== Polarity =====================
// Most relay modules are active LOW (LOW = relay ON).
constexpr bool RELAY_ACTIVE_LOW = true;
// Most flame modules read LOW when a flame is present.
constexpr bool FLAME_ACTIVE_LOW = true;

// ===================== Timing (milliseconds) =====================
constexpr unsigned long FLAME_CONFIRM_MS = 500;    // debounce before alarm
constexpr unsigned long FLAME_CLEAR_MS   = 5000;   // keep pump running after flame clears
constexpr unsigned long STATUS_PUBLISH_MS = 2000;  // dashboard update interval
constexpr unsigned long SENSOR_POLL_MS   = 50;     // sensor sampling interval
constexpr unsigned long WIFI_RETRY_MS    = 10000;  // Wi-Fi reconnect interval
constexpr unsigned long SERIAL_BAUD      = 115200;

// ===================== Blynk Virtual Pins =====================
constexpr int VPIN_FIRE_STATUS   = 0;  // V0 — LED widget: fire ON/OFF
constexpr int VPIN_PUMP_STATUS   = 1;  // V1 — LED widget: pump ON/OFF
constexpr int VPIN_DEVICE_ONLINE = 2;  // V2 — LED widget: device online
constexpr int VPIN_PUMP_MANUAL   = 3;  // V3 — Button: manual pump override

// ===================== Shared Enums =====================
// System-wide fire state machine.
enum class SystemState { NORMAL, FIRE, COOLDOWN };

#endif  // CONSTANTS_H
