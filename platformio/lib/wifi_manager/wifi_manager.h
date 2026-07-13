#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <Arduino.h>

/*
 * wifi_manager — connects and keeps the ESP32 on Wi-Fi.
 * Non-blocking: call loop() from the main loop. It never stalls the
 * fire logic, which must keep running even with no network.
 */
namespace WifiManager {
void begin();
void loop();
bool isConnected();
String ipAddress();
}  // namespace WifiManager

#endif  // WIFI_MANAGER_H
