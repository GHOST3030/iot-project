/*
 * config.example.h — credentials template.
 *
 * Copy this file to config.h and fill in your values:
 *     cp lib/config/config.example.h lib/config/config.h
 *
 * config.h is git-ignored — never commit real credentials.
 *
 * For Wokwi simulation, use the built-in virtual network:
 *     WIFI_SSID = "Wokwi-GUEST"   WIFI_PASSWORD = ""
 * On real hardware, use your own Wi-Fi.
 */
#ifndef CONFIG_H
#define CONFIG_H

// ---- Wi-Fi ----
#define WIFI_SSID       "Wokwi-GUEST"
#define WIFI_PASSWORD   ""

// ---- Blynk IoT (https://blynk.cloud) ----
// These three must be defined before including the Blynk library.
#define BLYNK_TEMPLATE_ID    "TMPLxxxxxxxx"
#define BLYNK_TEMPLATE_NAME  "Fire Detection System"
#define BLYNK_AUTH_TOKEN     "YOUR_BLYNK_AUTH_TOKEN"

#endif  // CONFIG_H
