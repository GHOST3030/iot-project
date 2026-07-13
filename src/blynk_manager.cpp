// config.h must come first: Blynk's headers require
// BLYNK_TEMPLATE_ID / BLYNK_TEMPLATE_NAME to be defined
// BEFORE BlynkSimpleEsp32.h is included.
#include "config.h"

#include <Arduino.h>
#include "constants.h"
#include "blynk_manager.h"
#include "wifi_manager.h"

#if ENABLE_BLYNK

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// Latest value of the dashboard's Manual Pump switch (V2).
static bool manualPumpRequested = false;

static unsigned long lastTelemetryTime = 0;
static unsigned long lastReconnectTime = 0;

// Called by the Blynk library whenever the app writes to V2.
BLYNK_WRITE(V2) {
  manualPumpRequested = (param.asInt() == 1);
  Serial.print("[BLYNK] Manual pump switch: ");
  Serial.println(manualPumpRequested ? "ON" : "OFF");
}

// Called when the cloud connection is (re)established: pull the
// current switch position so device and app agree after a reboot.
BLYNK_CONNECTED() {
  Serial.println("[BLYNK] Connected to Blynk cloud.");
  Blynk.syncVirtual(VPIN_MANUAL_PUMP);
}

void blynkSetup() {
  if (!wifiIsConnected()) {
    Serial.println("[BLYNK] Skipped - no Wi-Fi connection.");
    return;
  }
  // config() + connect() instead of Blynk.begin(): begin() blocks
  // FOREVER if the cloud is unreachable, which would freeze the
  // fire detection loop. connect() gives up after its timeout.
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect();
}

void blynkUpdate(bool fireDetected, bool pumpOn, unsigned long now) {
  if (!wifiIsConnected()) {
    return;
  }

  if (!Blynk.connected()) {
    // Rate-limited reconnect so a cloud outage can't spam-block the loop.
    if (now - lastReconnectTime >= BLYNK_RECONNECT_INTERVAL_MS) {
      lastReconnectTime = now;
      Serial.println("[BLYNK] Reconnecting...");
      Blynk.connect();
    }
    return;
  }

  // Processes incoming commands (e.g. the V2 switch) and keepalives.
  Blynk.run();

  // Push telemetry at a fixed rate, not on every loop pass.
  if (now - lastTelemetryTime >= TELEMETRY_INTERVAL_MS) {
    lastTelemetryTime = now;
    Blynk.virtualWrite(VPIN_FIRE_STATUS, fireDetected ? 1 : 0);
    Blynk.virtualWrite(VPIN_PUMP_STATUS, pumpOn ? 1 : 0);
    Blynk.virtualWrite(VPIN_UPTIME, now / 1000);
  }
}

bool blynkManualPumpRequested() {
  return manualPumpRequested;
}

#else // ENABLE_BLYNK == 0: offline stubs, zero cloud dependency.

void blynkSetup() {
  Serial.println("[BLYNK] Disabled in config.h (ENABLE_BLYNK = 0).");
}

void blynkUpdate(bool fireDetected, bool pumpOn, unsigned long now) {
  (void)fireDetected;
  (void)pumpOn;
  (void)now;
}

bool blynkManualPumpRequested() {
  return false;
}

#endif // ENABLE_BLYNK
