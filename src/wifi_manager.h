#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

// ============================================================
// wifi_manager — connects the ESP32 to Wi-Fi and reports status.
//
// Design decision: the connection attempt is allowed to block
// during setup() (with a hard timeout) because nothing else is
// running yet. If Wi-Fi is unavailable the system keeps working
// OFFLINE — fire detection must never depend on the network.
// ============================================================

// Try to connect. Returns true on success, false after
// WIFI_CONNECT_TIMEOUT_MS. Call once from setup().
bool wifiConnect();

// True while the ESP32 is associated with the access point.
bool wifiIsConnected();

// Print SSID, status and IP address to the serial monitor.
void wifiPrintStatus();

#endif // WIFI_MANAGER_H
