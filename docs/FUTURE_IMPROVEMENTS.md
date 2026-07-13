# Future Improvements (post-MVP roadmap)

Ordered roughly by value ÷ effort. Each item is deliberately **excluded** from the MVP to keep it simple and finishable.

## v1.x — small firmware upgrades

1. **Alarm latching + silence button** — keep the alarm active until a human acknowledges it (real fire panels latch). Adds one state (`ALARM_LATCHED`) and one Blynk button.
2. **Pump run-time limit** — auto-stop the pump after N seconds to protect it from running dry; adds a `COOLDOWN` state.
3. **Analog flame reading (AO pin)** — read flame *intensity* on an ADC pin (GPIO34) instead of just on/off; enables a severity gauge on the dashboard.
4. **Blynk push notifications** — `Blynk.logEvent("fire_alert")` so phones get a notification, not just a dashboard change.
5. **Wi-Fi auto-reconnect** — periodic reconnect attempts in `loop()` (currently connection is attempted only at boot).

## v2 — more sensing, more trust

6. **Smoke sensor (MQ-2) + temperature (DHT22)** — fire confirmation by two independent signals drastically cuts false positives; classic sensor-fusion exercise.
7. **Local fallback alarm schedule** — distinct beep patterns for "sensor fault" vs "fire" (self-diagnostics: detect a disconnected sensor).
8. **Persistent event log** — store the last N alarms in ESP32 NVS/flash with timestamps (needs NTP time sync).
9. **OTA firmware updates** — `ArduinoOTA` or Blynk.Air, so deployed devices update without USB.

## v3 — productization

10. **Multiple zones** — several flame sensors, per-zone pump valves; the moment to refactor modules into C++ classes with instances.
11. **Local web dashboard** — ESP32 serves a status page on its IP (works with no cloud at all).
12. **MQTT + Home Assistant** — replace/augment Blynk with an open protocol.
13. **Battery + deep sleep design** — for battery operation, wake-on-interrupt from the flame sensor pin.
14. **Watchdog + brown-out hardening, PCB design** — from breadboard to a real product.
