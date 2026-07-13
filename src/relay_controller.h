#ifndef RELAY_CONTROLLER_H
#define RELAY_CONTROLLER_H

// ============================================================
// relay_controller — drives the relay that switches the water
// pump. The pump itself is NEVER driven by the ESP32 directly:
// the relay isolates the low-voltage logic from the pump's
// power circuit (in Wokwi the "pump" is a green LED behind the
// relay contacts; on real hardware it is a 5-12 V pump).
// ============================================================

// Configure the relay pin. Call once from setup().
void relaySetup();

// Energize / de-energize the relay (true = pump running).
void relaySetPump(bool on);

// Current pump state, used for logging and the dashboard.
bool relayIsPumpOn();

#endif // RELAY_CONTROLLER_H
