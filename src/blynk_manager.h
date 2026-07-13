#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

// ============================================================
// blynk_manager — the ONLY module that talks to the Blynk cloud.
//
// Dashboard datastreams (see constants.h):
//   V0 Fire Status (LED widget)     — device -> app
//   V1 Pump Status (LED widget)     — device -> app
//   V2 Manual Pump (switch widget)  — app -> device
//   V3 Uptime seconds (label)       — device -> app ("online" proof)
//
// When ENABLE_BLYNK is 0 (config.h) every function compiles to a
// harmless stub, so the rest of the firmware never has to know
// whether the cloud is in use. This keeps Blynk fully decoupled.
// ============================================================

// Configure the Blynk connection. Call once from setup(),
// AFTER Wi-Fi is up.
void blynkSetup();

// Keep the connection alive and push telemetry (throttled to
// TELEMETRY_INTERVAL_MS). Call every loop() pass.
void blynkUpdate(bool fireDetected, bool pumpOn, unsigned long now);

// True while the dashboard's Manual Pump switch (V2) is ON.
bool blynkManualPumpRequested();

#endif // BLYNK_MANAGER_H
