#ifndef CONFIG_H
#define CONFIG_H

// ============================================================
// config.h — Everything you may need to CHANGE lives here.
//
// Moving from Wokwi simulation to a real ESP32?
// This is the ONLY file you should have to touch:
//   1. Update the pin numbers if your wiring differs.
//   2. Put your real Wi-Fi credentials in.
//   3. Put your Blynk template/token in and set ENABLE_BLYNK 1.
// ============================================================

// ------------------------------------------------------------
// Pin mapping (ESP32 DevKit V1)
// ------------------------------------------------------------
// Flame sensor digital output (DO).
// In Wokwi a slide switch simulates it: switch closed -> pin LOW -> "fire".
// Real flame sensor modules also pull DO LOW when a flame is seen,
// so the same code works on real hardware.
#define PIN_FLAME_SENSOR 27

// Red alarm LED (blinks while fire is detected).
#define PIN_ALARM_LED 26

// Active buzzer (beeps while fire is detected).
#define PIN_BUZZER 25

// Relay module input. The relay switches the water pump.
// In Wokwi the "pump" is a green LED wired through the relay contacts.
#define PIN_RELAY 33

// ------------------------------------------------------------
// Wi-Fi credentials
// ------------------------------------------------------------
// "Wokwi-GUEST" is the built-in open network of the Wokwi simulator.
// On real hardware, replace with your own SSID / password.
#define WIFI_SSID "Wokwi-GUEST"
#define WIFI_PASSWORD ""

// ------------------------------------------------------------
// Blynk (IoT dashboard)
// ------------------------------------------------------------
// Set ENABLE_BLYNK to 1 AFTER you create a Blynk template and
// paste the three values below from the Blynk console.
// With 0 the firmware builds and runs fully offline-capable,
// which lets you test steps 1-5 before touching the cloud.
#define ENABLE_BLYNK 0

#define BLYNK_TEMPLATE_ID "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME "Smart Fire Detection"
#define BLYNK_AUTH_TOKEN "your-blynk-auth-token"

#endif // CONFIG_H
