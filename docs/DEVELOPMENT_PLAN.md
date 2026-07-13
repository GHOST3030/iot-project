# Development Plan — Incremental, Simulation-First

This is the plan the project was built with, and the plan you should follow if you rebuild it yourself to learn. **Never move to the next step until the current step's test passes.** Each step leaves the project in a working state.

The golden rule of embedded development: *change one thing, test it, commit it.*

---

## Step 0 — Project skeleton

**Do:** Create the PlatformIO project (`platformio.ini`, `src/`, `include/`), `wokwi.toml`, an empty-loop `main.cpp`, and `.gitignore`.

**Why PlatformIO and not the Arduino IDE?** PlatformIO gives us a real folder structure (the Arduino IDE fights against multiple files), dependency management (`lib_deps`), and works with the Wokwi VS Code extension. The code is still 100 % Arduino C++.

**Test:** `pio run` builds with zero errors. Wokwi starts and the serial monitor prints the boot banner.

---

## Step 1 — Alarm LED (`led_controller`)

**Do:** Create `led_controller.h/.cpp` with `ledSetup()`, `ledBlink(now)`, `ledOff()`. Blink it from `loop()` temporarily.

**Why start with an LED?** It's the embedded "hello world": it proves the toolchain, the board, the pin, and our `millis()` timing pattern — with the simplest possible output.

**Why `millis()` and not `delay()`?** `delay(250)` freezes the CPU: while blinking, it could not read the flame sensor. The `millis()` pattern ("has enough time passed since I last acted?") keeps the loop spinning thousands of times per second, so every module stays responsive. Every time-based behavior in this project uses the same pattern.

**Test (Wokwi):** Red LED blinks steadily at 2 Hz. Nothing else on the board reacts.

---

## Step 2 — Flame sensor (`flame_sensor`)

**Do:** Create `flame_sensor.h/.cpp`: poll GPIO27 every 50 ms, debounce 3 consecutive identical readings, expose `flameSensorFireDetected()`. In `loop()`, print state changes and blink the LED only while fire is detected.

**Why a slide switch in simulation?** Wokwi has no flame-sensor part. A flame sensor module's digital output is just a pin that goes LOW when flame is seen — a slide switch to GND behaves identically. The firmware cannot tell the difference, which is exactly why the code transfers to real hardware unmodified.

**Why debounce?** Real sensor outputs jitter near the detection threshold (and switches physically bounce). Requiring 3 stable readings (150 ms) trades a tiny detection delay for zero false alarms — a good deal for a fire system.

**Test (Wokwi):** Slide the switch → within ~0.2 s the serial monitor prints the state change and the LED starts blinking. Slide back → LED off. Repeat rapidly to confirm no flickering false states.

---

## Step 3 — Buzzer + Relay/Pump (`buzzer_controller`, `relay_controller`)

**Do:** Create both modules. Buzzer beeps 500 ms on/off during fire; relay energizes during fire, lighting the green "pump" LED through its COM/NO contacts.

**Why does the pump sit behind a relay even in simulation?** Because that's the real-world safety architecture: a pump draws far more current than a GPIO pin can source, and the relay galvanically separates the pump's supply from the microcontroller. Simulating the same topology means the wiring diagram is also the production wiring diagram.

**Why does `relaySetPump()` ignore repeated identical calls?** `applyOutputs()` runs every loop pass (thousands of times/second). The guard means the hardware write and the serial log happen only on actual changes — otherwise the log would be unreadable.

**Test (Wokwi):** Switch to FIRE → LED blinks, buzzer beeps, relay clicks, green pump LED lights. Switch back → all off, serial shows `[RELAY] Water pump: OFF` exactly once.

---

## Step 4 — State machine (`main.cpp`)

**Do:** Introduce the `SystemState` enum and restructure `loop()` into the fixed pipeline: **sense → decide → act → report**. All output decisions move into `applyOutputs(state)`.

**Why a state machine instead of `if (fire) {...}`?** With two inputs (sensor + future manual switch) the number of combinations grows. A state machine makes the system's behavior *enumerable*: every state has a defined output set, every transition is logged. Adding a state later (e.g. `COOLDOWN`) is a local change instead of a rewrite.

**Test (Wokwi):** Serial shows clean transitions: `[STATE] IDLE -> FIRE_ALARM`, `[STATE] FIRE_ALARM -> IDLE`. Outputs always match the table in the README. No transition spam while the switch is held.

---

## Step 5 — Wi-Fi (`wifi_manager`)

**Do:** Create `wifi_manager.h/.cpp`. Connect to `Wokwi-GUEST` in `setup()`, print progress dots, the status, and the IP address. Hard timeout of 15 s.

**Why is a blocking connect acceptable here?** It runs in `setup()`, before monitoring starts, and it *cannot exceed 15 s*. The alternative (fully async connection) adds real complexity for no MVP benefit. The critical design decision is different: **on failure the system continues offline** — a fire detector that bricks itself without internet would be a broken product.

**Test (Wokwi):** Serial prints `Connecting to "Wokwi-GUEST" ...`, then `CONNECTED` and an IP like `10.13.37.2`. Then break it on purpose: change the SSID to a wrong name, rerun, and confirm the system says `FAILED - continuing in OFFLINE mode` and **fire detection still works**. Restore the SSID.

---

## Step 6 — Blynk dashboard (`blynk_manager`)

**Do:** Create `blynk_manager.h/.cpp` behind the `ENABLE_BLYNK` flag. Datastreams V0–V3, telemetry every 1 s, `BLYNK_WRITE(V2)` handler for the manual pump switch, `MANUAL_PUMP` state in the state machine.

**Why an `ENABLE_BLYNK` compile flag?** Two reasons. (1) Learning: steps 0–5 should be testable before creating any cloud account. (2) Architecture: it forces the cloud code to stay decoupled — the rest of the firmware compiles and runs identically with Blynk absent, proving no module secretly depends on it.

**Why `Blynk.config()` + `Blynk.connect()` instead of `Blynk.begin()`?** `Blynk.begin()` blocks *forever* until it reaches the cloud. A cloud outage would freeze fire monitoring — unacceptable. `connect()` has a timeout, and reconnects are rate-limited in `blynkUpdate()`.

**Why is fire checked before the manual switch in `computeNextState()`?** Priority encoding: a remote user must never be able to mask a real alarm. The order of the `if` statements *is* the safety policy.

**Test (Wokwi):** With real credentials pasted and `ENABLE_BLYNK 1`: dashboard shows Device Online, uptime counting, Fire/Pump LEDs mirror the simulation within 1 s. Toggle the V2 switch → pump LED (only) turns on, state `MANUAL_PUMP`. Trigger fire while V2 is off → full alarm. Confirm the V2 switch cannot cancel an active fire alarm.

---

## Step 7 — Documentation & handoff

**Do:** README, architecture doc, testing checklist, future improvements. Verify a stranger can clone → build → simulate using only the README.

**Test:** Run the full [TESTING_CHECKLIST.md](TESTING_CHECKLIST.md) top to bottom in one sitting.
