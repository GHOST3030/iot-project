#ifndef CONFIG_H
#define CONFIG_H

// ===================== Pin Assignments =====================
#define PIN_FLAME_DIGITAL   34   // Flame sensor DO (input-only pin)
#define PIN_FLAME_ANALOG    35   // Flame sensor AO (input-only pin)
#define PIN_RELAY           26   // Relay IN (controls water pump)
#define PIN_BUZZER          27   // Active buzzer
#define PIN_LED_RED         25   // Alarm indicator
#define PIN_LED_GREEN       33   // Normal-state indicator

// ===================== Relay / Sensor Logic =====================
// Most relay modules are active LOW: LOW = relay ON.
#define RELAY_ACTIVE_LOW    true
// Most flame sensor modules output LOW when flame is detected.
#define FLAME_ACTIVE_LOW    true

// ===================== Timing (milliseconds) =====================
#define FLAME_CONFIRM_MS    500    // Flame must persist this long before alarm (debounce)
#define FLAME_CLEAR_MS      5000   // No flame for this long before stopping the pump
#define STATUS_PUBLISH_MS   2000   // How often to publish status
#define SENSOR_POLL_MS      50     // Sensor sampling interval
#define WIFI_RETRY_MS       10000  // Wi-Fi reconnect attempt interval
#define MQTT_RETRY_MS       5000   // MQTT reconnect attempt interval

// ===================== MQTT Topics =====================
// Published by the device (status):
#define TOPIC_STATE         "firesystem/state"         // NORMAL / FIRE / COOLDOWN (retained)
#define TOPIC_FLAME         "firesystem/flame"         // analog intensity 0-4095
#define TOPIC_PUMP          "firesystem/pump"          // ON / OFF (retained)
#define TOPIC_MODE          "firesystem/mode"          // AUTO / MANUAL (retained)
#define TOPIC_ALERT         "firesystem/alert"         // fire alert messages
#define TOPIC_AVAILABILITY  "firesystem/availability"  // online / offline (LWT, retained)

// Subscribed by the device (commands from the app):
#define TOPIC_CMD_PUMP      "firesystem/cmd/pump"      // ON / OFF (manual mode only)
#define TOPIC_CMD_MODE      "firesystem/cmd/mode"      // AUTO / MANUAL

#define MQTT_CLIENT_ID      "esp32-fire-system"

#endif // CONFIG_H
