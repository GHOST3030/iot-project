/*
 * IoT Fire Detection & Suppression System — Wokwi Simulation
 * -----------------------------------------------------------
 * A self-contained version for the Wokwi simulator (wokwi.com).
 * No Wi-Fi / cloud needed — press "play" and it runs.
 *
 * The IR flame sensor is simulated with a pushbutton:
 *   - Button PRESSED  = flame detected
 *   - Button RELEASED = no flame
 * (On real hardware the flame module drives this line instead;
 *  the state-machine logic is identical to the Blynk/MQTT versions.)
 *
 * The relay module stands in for the water pump.
 *
 * How to run:
 *   1. Go to https://wokwi.com → New Project → ESP32.
 *   2. Replace sketch.ino with this file and diagram.json with ours.
 *   3. Click the green play button. Open the Serial Monitor.
 *   4. Press and hold the "Flame" button to simulate a fire.
 */

// ===================== Pin Assignments =====================
#define PIN_FLAME_DIGITAL   34   // Flame button (LOW = flame)
#define PIN_RELAY           26   // Relay IN (water pump)
#define PIN_BUZZER          27   // Buzzer
#define PIN_LED_RED         25   // Alarm indicator
#define PIN_LED_GREEN       33   // Normal-state indicator

#define RELAY_ACTIVE_LOW    true // most relay modules: LOW = ON

// ===================== Timing (milliseconds) =====================
#define FLAME_CONFIRM_MS    500    // flame must persist before alarm (debounce)
#define FLAME_CLEAR_MS      5000   // no flame this long before stopping the pump
#define STATUS_PRINT_MS     1000   // how often to print status
#define SENSOR_POLL_MS      50     // sensor sampling interval

// ===================== System State =====================
enum SystemState { STATE_NORMAL, STATE_FIRE, STATE_COOLDOWN };
SystemState state = STATE_NORMAL;

bool pumpOn = false;
unsigned long flameFirstSeenAt = 0;
unsigned long flameLastSeenAt  = 0;
unsigned long lastSensorPoll   = 0;
unsigned long lastStatusPrint  = 0;

// ===================== Hardware Helpers =====================
void setPump(bool on) {
  pumpOn = on;
  digitalWrite(PIN_RELAY, (RELAY_ACTIVE_LOW ? !on : on) ? HIGH : LOW);
}

void setAlarm(bool on) {
  digitalWrite(PIN_BUZZER, on ? HIGH : LOW);
  digitalWrite(PIN_LED_RED, on ? HIGH : LOW);
  digitalWrite(PIN_LED_GREEN, on ? LOW : HIGH);
}

bool flameDetected() {
  // Button uses INPUT_PULLUP: pressed pulls the pin LOW = flame.
  return digitalRead(PIN_FLAME_DIGITAL) == LOW;
}

const char* stateName() {
  switch (state) {
    case STATE_FIRE:     return "FIRE";
    case STATE_COOLDOWN: return "COOLDOWN";
    default:             return "NORMAL";
  }
}

// ===================== Fire State Machine =====================
void runLogic(unsigned long now, bool flame) {
  if (flame) {
    flameLastSeenAt = now;
    if (flameFirstSeenAt == 0) flameFirstSeenAt = now;
  } else {
    flameFirstSeenAt = 0;
  }

  switch (state) {
    case STATE_NORMAL:
      if (flame && now - flameFirstSeenAt >= FLAME_CONFIRM_MS) {
        state = STATE_FIRE;
        setAlarm(true);
        setPump(true);
        Serial.println(">>> FIRE DETECTED — alarm + pump ON");
      }
      break;

    case STATE_FIRE:
      if (!flame && now - flameLastSeenAt >= FLAME_CLEAR_MS) {
        state = STATE_COOLDOWN;
        setPump(false);
        setAlarm(false);
        Serial.println("Flame cleared — pump OFF, cooling down");
      }
      break;

    case STATE_COOLDOWN:
      if (flame) {
        state = STATE_FIRE;
        setAlarm(true);
        setPump(true);
        Serial.println("Re-ignition! Alarm + pump back ON");
      } else if (now - flameLastSeenAt >= 2 * FLAME_CLEAR_MS) {
        state = STATE_NORMAL;
        Serial.println("System back to NORMAL");
      }
      break;
  }
}

void printStatus(unsigned long now) {
  if (now - lastStatusPrint < STATUS_PRINT_MS) return;
  lastStatusPrint = now;
  Serial.printf("[%s] flame=%s pump=%s\n",
                stateName(),
                flameDetected() ? "YES" : "no",
                pumpOn ? "ON" : "OFF");
}

// ===================== Setup & Loop =====================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_FLAME_DIGITAL, INPUT_PULLUP);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  setPump(false);
  setAlarm(false);

  Serial.println("Fire detection system (Wokwi simulation) started");
  Serial.println("Press and hold the Flame button to simulate a fire.");
}

void loop() {
  unsigned long now = millis();

  if (now - lastSensorPoll >= SENSOR_POLL_MS) {
    lastSensorPoll = now;
    runLogic(now, flameDetected());
  }

  printStatus(now);
}
