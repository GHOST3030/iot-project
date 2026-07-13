#ifndef BLYNK_MANAGER_H
#define BLYNK_MANAGER_H

/*
 * blynk_manager — the ONLY module that includes the Blynk library.
 * It publishes system status to the phone dashboard and exposes the
 * manual pump-override button back to the main logic.
 *
 * Kept non-blocking: connects in the background so the fire logic
 * runs with or without a cloud connection.
 */
namespace BlynkManager {
void begin();
void loop();
bool isConnected();

// Push current status to the dashboard (fire, pump, online LEDs).
void publishStatus(bool fireActive, bool pumpOn);

// Fire a one-shot push notification to the phone.
void notifyFire();

// True while the user is holding the manual pump button in the app.
bool manualPumpOverride();
}  // namespace BlynkManager

#endif  // BLYNK_MANAGER_H
