# Testing Checklist

Run top to bottom. Every box must pass before the project is considered working. Items marked 🖥 apply to Wokwi simulation, 🔩 to real hardware, none = both.

## 1. Build & Boot

- [ ] `pio run` completes with zero errors and zero warnings from project code
- [ ] Serial monitor (115200 baud) shows the boot banner
- [ ] `[BLYNK] Disabled...` line appears when `ENABLE_BLYNK` is 0
- [ ] `[SYSTEM] Monitoring for fire...` appears — boot never hangs

## 2. Idle State

- [ ] Alarm LED off, buzzer silent, relay released, pump LED off
- [ ] No repeated log output while idle (state is quiet, not spamming)

## 3. Fire Detection

- [ ] 🖥 Sliding the flame switch triggers the alarm in under ~0.5 s
- [ ] 🔩 A lighter held ~20 cm from the sensor triggers the alarm (adjust the module's potentiometer for sensitivity)
- [ ] Serial prints `[STATE] IDLE -> FIRE_ALARM` and the `*** FIRE DETECTED ***` banner exactly once per event
- [ ] Alarm LED blinks (~2 Hz), buzzer beeps (~1 Hz), relay clicks on, pump (LED) runs
- [ ] Rapidly toggling the switch does NOT produce flickering states (debounce works)

## 4. Fire Cleared

- [ ] Clearing the flame returns the system to `IDLE`
- [ ] All outputs off; `[RELAY] Water pump: OFF` printed exactly once
- [ ] A second fire event immediately re-triggers the full alarm (system is re-armed)

## 5. Wi-Fi

- [ ] Serial shows progress dots, then `CONNECTED` and a valid IP address
- [ ] 🖥 Connects to `Wokwi-GUEST` within a few seconds
- [ ] **Failure path:** with a wrong SSID, boot continues after 15 s with `OFFLINE mode`, and fire detection still works fully
- [ ] Restore correct SSID afterwards

## 6. Blynk Dashboard (`ENABLE_BLYNK 1` + real credentials)

- [ ] Device shows **Online** in the Blynk console
- [ ] Uptime label (V3) increments every second
- [ ] Fire Status LED (V0) turns on within ~1 s of a fire event
- [ ] Pump Status LED (V1) mirrors the relay state
- [ ] Manual Pump switch (V2) ON → pump runs, state `MANUAL_PUMP`, **no** LED/buzzer
- [ ] Manual Pump switch OFF → pump stops, back to `IDLE`
- [ ] **Safety:** during a real fire alarm, toggling V2 does NOT stop the alarm or the pump
- [ ] After rebooting the device with V2 left ON, the pump resumes (state sync via `BLYNK_CONNECTED`)
- [ ] Disconnect scenario: with Blynk unreachable, the loop stays responsive (reconnect attempts are visible at most every 10 s, fire detection unaffected)

## 7. Real-Hardware Extras 🔩

- [ ] Relay board polarity verified (if pump runs at boot, flip `RELAY_ACTIVE_LEVEL`)
- [ ] Pump powered from its own supply through relay COM/NO — never from a GPIO
- [ ] Common ground between ESP32 and relay board
- [ ] Flame sensor VCC on 3V3 (its DO must not exceed 3.3 V)
- [ ] System runs 30+ minutes without a watchdog reset or freeze
