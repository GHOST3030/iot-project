# 🔥 Smart Fire Detection & Water Extinguishing System (MVP)

An ESP32 + Arduino C++ project that continuously monitors for fire and automatically activates a water pump, buzzer, and alarm LED — with a Blynk IoT dashboard for remote monitoring and manual pump control.

**Simulation-first:** the whole project runs in [Wokwi](https://wokwi.com) with zero hardware. Moving to a real ESP32 later requires editing **only `include/config.h`** (pins + credentials).

---

## Documentation Map

| Document | What's inside |
|---|---|
| [docs/DEVELOPMENT_PLAN.md](docs/DEVELOPMENT_PLAN.md) | The incremental build plan — 7 steps, each with its own test |
| [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md) | System workflow, state machine diagram, component explanations |
| [docs/TESTING_CHECKLIST.md](docs/TESTING_CHECKLIST.md) | Full verification checklist for simulation and real hardware |
| [docs/FUTURE_IMPROVEMENTS.md](docs/FUTURE_IMPROVEMENTS.md) | Ideas for v2 and beyond |

---

## Folder Structure

```
iot-project/
├── platformio.ini            # Build configuration (board, framework, libraries)
├── wokwi.toml                # Wokwi simulator config (points at the built firmware)
├── diagram.json              # Wokwi circuit (parts + wiring)
├── include/
│   ├── config.h              # ★ Pins, Wi-Fi, Blynk credentials — the ONLY file to
│   │                         #   edit when moving to real hardware
│   └── constants.h           # Named constants, timings, system state enum
├── src/
│   ├── main.cpp              # Setup + main loop + state machine (no hardware details)
│   ├── flame_sensor.h/.cpp   # Debounced flame sensor reading
│   ├── led_controller.h/.cpp # Alarm LED (millis()-based blink)
│   ├── buzzer_controller.h/.cpp # Buzzer (millis()-based beep pattern)
│   ├── relay_controller.h/.cpp  # Relay → water pump switching
│   ├── wifi_manager.h/.cpp   # Wi-Fi connection + status reporting
│   └── blynk_manager.h/.cpp  # Blynk dashboard (fully decoupled, can be disabled)
└── docs/                     # Plan, architecture, testing, roadmap
```

**Why this layout?** Each module owns exactly one piece of hardware (or one external service) behind a tiny 2–3 function API. `main.cpp` only orchestrates — it never calls `digitalWrite` itself. That is what makes the code portable, testable, and easy to extend.

---

## Pin Mapping

| Component | ESP32 GPIO | Direction | Notes |
|---|---|---|---|
| Flame sensor (DO) | **27** | Input (pull-up) | Active **LOW** — pin goes LOW when flame detected |
| Alarm LED (red) | **26** | Output | Through 220 Ω resistor |
| Buzzer | **25** | Output | Active buzzer, HIGH = sound |
| Relay IN | **33** | Output | Active HIGH (configurable in `constants.h`) |
| Water pump | — | via relay | Green LED behind relay contacts in simulation |

All pins are defined once in `include/config.h`.

## Wiring Diagram

```
                 ESP32 DevKit V1
                ┌────────────────┐
 Flame sensor   │                │
 (slide switch  │ GPIO27 ◄───────┼──── switch ──── GND     (closed = FIRE)
  in Wokwi)     │                │
                │ GPIO26 ────────┼──── 220Ω ──►│ RED LED ──── GND   (alarm)
                │                │
                │ GPIO25 ────────┼──── BUZZER(+)   BUZZER(−) ── GND
                │                │
                │ GPIO33 ────────┼──── IN  ┌──────────────┐
                │ VIN (5V) ──────┼──── VCC │ RELAY MODULE │
                │ GND ───────────┼──── GND └──┬────────┬──┘
                │                │           COM       NO
                │ 3V3 ───────────┼────────────┘        └──►│ GREEN LED ── 220Ω ── GND
                └────────────────┘                              ("water pump")
```

The relay's COM/NO contacts switch the pump circuit — the ESP32 never powers the pump directly. On real hardware, replace the green LED with the pump + its own 5–12 V supply.

---

## Quick Start (Simulation — no hardware needed)

### Option A — VS Code + PlatformIO + Wokwi extension (recommended)

1. Install [VS Code](https://code.visualstudio.com/), the **PlatformIO IDE** extension, and the **Wokwi Simulator** extension.
2. Clone this repo and open the folder in VS Code.
3. Build: `pio run` (or the PlatformIO ✓ button). PlatformIO downloads the ESP32 toolchain and the Blynk library automatically.
4. Press `F1` → **Wokwi: Start Simulator**.
5. Watch the serial monitor: boot banner → Wi-Fi connects to `Wokwi-GUEST` → IP address printed.
6. **Click the slide switch** → FIRE: red LED blinks, buzzer beeps, relay clicks, green "pump" LED lights.
7. Slide it back → everything turns off.

### Option B — wokwi.com in the browser

1. Create a new **ESP32** project at [wokwi.com](https://wokwi.com).
2. Copy `diagram.json` into the diagram tab.
3. Add each `src/*.cpp`/`.h` and `include/*.h` file as a tab (in browser sketches, `#include "config.h"` works when all files sit side-by-side).
4. In the Library Manager tab add **Blynk** (only needed once `ENABLE_BLYNK` is 1).
5. Press ▶.

### Enabling the Blynk dashboard

1. Create a free account at [blynk.cloud](https://blynk.cloud) → New Template (ESP32, WiFi).
2. Add datastreams: `V0` Fire Status (int 0–1), `V1` Pump Status (int 0–1), `V2` Manual Pump (int 0–1), `V3` Uptime (int).
3. Build a web/mobile dashboard: two LED widgets (V0, V1), a Switch (V2), a Label (V3).
4. Create a device from the template and copy the three credentials into `include/config.h`, then set `ENABLE_BLYNK 1`.
5. Rebuild and run — Wokwi's virtual Wi-Fi (`Wokwi-GUEST`) reaches the real Blynk cloud, so the dashboard works **from inside the simulation**.

---

## Moving to Real Hardware

1. Buy: ESP32 DevKit V1, flame sensor module (e.g. KY-026), 1-channel 5 V relay module, small 5 V water pump, active buzzer, red LED + 220 Ω resistor.
2. Wire exactly per the pin table above (flame sensor DO → GPIO27, its VCC/GND to 3V3/GND).
3. In `include/config.h`: set your Wi-Fi SSID/password; adjust pins only if your wiring differs.
4. If your relay board is **active-LOW**, flip `RELAY_ACTIVE_LEVEL` in `include/constants.h`.
5. Power the pump from its **own** supply through the relay contacts (COM/NO) — never from the ESP32's pins.
6. `pio run -t upload`, then `pio device monitor`.

That's the entire migration — the firmware logic is unchanged.

---

## System Behavior (summary)

| Condition | LED | Buzzer | Relay/Pump | State |
|---|---|---|---|---|
| No fire, switch off | OFF | OFF | OFF | `IDLE` |
| **Fire detected** | **Blinks** | **Beeps** | **ON** | `FIRE_ALARM` |
| No fire, Blynk manual switch ON | OFF | OFF | ON | `MANUAL_PUMP` |

Fire always has priority — the manual switch can never suppress an alarm. Full state machine in [docs/ARCHITECTURE.md](docs/ARCHITECTURE.md).
