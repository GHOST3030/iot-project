#include "blynk_manager.h"

// Blynk template identifiers must be defined before the library include.
// They live in config.h (copied from config.example.h).
#include "config.h"

#include <BlynkSimpleEsp32.h>

#include "constants.h"

namespace {
// Set from the app via the manual pump button (V3).
volatile bool g_manualPump = false;
}  // namespace

// ---- Blynk callback: manual pump override button (V3) ----
BLYNK_WRITE(VPIN_PUMP_MANUAL) {
  g_manualPump = (param.asInt() == 1);
  Serial.printf("Blynk: manual pump override = %s\n",
                g_manualPump ? "ON" : "OFF");
}

// Re-sync widget state whenever the app (re)connects.
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_PUMP_MANUAL);
  Blynk.virtualWrite(VPIN_DEVICE_ONLINE, 1);
}

namespace BlynkManager {

void begin() {
  // config() + connect() is non-blocking, unlike Blynk.begin().
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);
}

void loop() {
  if (Blynk.connected()) {
    Blynk.run();
  }
}

bool isConnected() { return Blynk.connected(); }

void publishStatus(bool fireActive, bool pumpOn) {
  if (!Blynk.connected()) return;
  Blynk.virtualWrite(VPIN_FIRE_STATUS, fireActive ? 1 : 0);
  Blynk.virtualWrite(VPIN_PUMP_STATUS, pumpOn ? 1 : 0);
  Blynk.virtualWrite(VPIN_DEVICE_ONLINE, 1);
}

void notifyFire() {
  if (!Blynk.connected()) return;
  Blynk.logEvent("fire_alert", "Fire detected! Pump activated.");
}

bool manualPumpOverride() { return g_manualPump; }

}  // namespace BlynkManager
