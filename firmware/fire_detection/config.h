#ifndef CONFIG_H
#define CONFIG_H

// ===================== Pin Assignments =====================
#define PIN_FLAME_DIGITAL   34   // Flame sensor DO (input-only pin)
#define PIN_FLAME_ANALOG    35   // Flame sensor AO (input-only pin)
#define PIN_RELAY           26   // Relay IN (controls water pump)
#define PIN_BUZZER          27   // Active buzzer
#define PIN_LED_RED         25   // Alarm indicator
#define PIN_LED_GREEN       33   // Normal-state indicator

// ===================== Relay Logic =====================
// Most relay modules are active LOW: LOW = relay ON.
// Set to false if your module is active HIGH.
#define RELAY_ACTIVE_LOW    true

// Most flame sensor modules output LOW when flame is detected.
// Set to false if yours outputs HIGH on flame.
#define FLAME_ACTIVE_LOW    true

// ===================== Timing (milliseconds) =====================
#define FLAME_CONFIRM_MS    500    // Flame must persist this long before alarm (debounce)
#define FLAME_CLEAR_MS      5000   // No flame for this long before stopping the pump
#define STATUS_PUBLISH_MS   2000   // How often to publish status to the app
#define SENSOR_POLL_MS      50     // Sensor sampling interval
#define WIFI_RETRY_MS       10000  // Wi-Fi reconnect attempt interval

// ===================== Blynk Virtual Pins =====================
#define VPIN_STATE          V0    // System state text (NORMAL / FIRE / COOLDOWN)
#define VPIN_FLAME_VALUE    V1    // Analog flame intensity reading
#define VPIN_PUMP_STATE     V2    // Pump ON/OFF indicator (LED widget)
#define VPIN_PUMP_SWITCH    V3    // Manual pump switch (works in MANUAL mode)
#define VPIN_MODE           V4    // 0 = AUTO, 1 = MANUAL

#endif // CONFIG_H
