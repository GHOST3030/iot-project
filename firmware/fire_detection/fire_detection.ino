/*
 * IoT Fire Detection & Suppression System — ESP32
 * ------------------------------------------------
 * - Monitors an IR flame sensor.
 * - On fire: turns on red LED + buzzer + water pump (via relay),
 *   and pushes a notification to the phone app (Blynk).
 * - Auto logic runs fully on the ESP32: it keeps working even if
 *   Wi-Fi or the cloud is down. The app adds monitoring + manual control.
 *
 * State machine:  NORMAL → FIRE → COOLDOWN → NORMAL
 *
 * Libraries (Arduino IDE → Library Manager):
 *   - Blynk (by Volodymyr Shymanskyy), v1.3+
 *
 * Before flashing: copy secrets.h.example → secrets.h and fill it in.
 */

#include "secrets.h"   // must come first: defines BLYNK_TEMPLATE_ID etc.
#include "config.h"

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

// ===================== System State =====================
enum SystemState { STATE_NORMAL, STATE_FIRE, STATE_COOLDOWN };
enum ControlMode { MODE_AUTO, MODE_MANUAL };

SystemState state = STATE_NORMAL;
ControlMode mode  = MODE_AUTO;

bool pumpOn            = false;
bool manualPumpCommand = false;   // last switch value from the app

unsigned long flameFirstSeenAt = 0;  // when flame was first detected (debounce)
unsigned long flameLastSeenAt  = 0;  // last time flame was seen (clear timer)
unsigned long lastSensorPoll   = 0;
unsigned long lastStatusPush   = 0;
unsigned long lastWifiAttempt  = 0;
bool fireNotified = false;           // avoid spamming notifications

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
  int reading = digitalRead(PIN_FLAME_DIGITAL);
  return FLAME_ACTIVE_LOW ? (reading == LOW) : (reading == HIGH);
}

const char* stateName() {
  switch (state) {
    case STATE_FIRE:     return "FIRE";
    case STATE_COOLDOWN: return "COOLDOWN";
    default:             return "NORMAL";
  }
}

// ===================== Blynk Callbacks =====================
// Manual pump switch from the app (only acts in MANUAL mode)
BLYNK_WRITE(VPIN_PUMP_SWITCH) {
  manualPumpCommand = param.asInt() == 1;
  if (mode == MODE_MANUAL) {
    setPump(manualPumpCommand);
  }
}

// Mode switch from the app: 0 = AUTO, 1 = MANUAL
BLYNK_WRITE(VPIN_MODE) {
  mode = (param.asInt() == 1) ? MODE_MANUAL : MODE_AUTO;
  Serial.printf("Mode changed to %s\n", mode == MODE_AUTO ? "AUTO" : "MANUAL");
  if (mode == MODE_MANUAL) {
    // Manual takes over: apply the last switch value
    setPump(manualPumpCommand);
  }
  // Returning to AUTO: the state machine reasserts control on next loop
}

// Sync widget states when the device (re)connects to the cloud
BLYNK_CONNECTED() {
  Blynk.syncVirtual(VPIN_PUMP_SWITCH, VPIN_MODE);
}

// ===================== Fire State Machine (AUTO mode) =====================
void runAutoLogic(unsigned long now, bool flame) {
  if (flame) {
    flameLastSeenAt = now;
    if (flameFirstSeenAt == 0) flameFirstSeenAt = now;
  } else {
    flameFirstSeenAt = 0;
  }

  switch (state) {
    case STATE_NORMAL:
      // Require the flame to persist before alarming (debounce)
      if (flame && now - flameFirstSeenAt >= FLAME_CONFIRM_MS) {
        state = STATE_FIRE;
        setAlarm(true);
        setPump(true);
        Serial.println("!!! FIRE DETECTED — pump ON");
        if (Blynk.connected() && !fireNotified) {
          Blynk.logEvent("fire_alert", "🔥 Fire detected! Pump activated.");
          fireNotified = true;
        }
      }
      break;

    case STATE_FIRE:
      // Flame gone → keep spraying through a safety window first
      if (!flame && now - flameLastSeenAt >= FLAME_CLEAR_MS) {
        state = STATE_COOLDOWN;
        setPump(false);
        setAlarm(false);
        Serial.println("Flame cleared — pump OFF, cooling down");
      }
      break;

    case STATE_COOLDOWN:
      // Re-ignition during cooldown goes straight back to FIRE
      if (flame) {
        state = STATE_FIRE;
        setAlarm(true);
        setPump(true);
        Serial.println("Re-ignition! Pump back ON");
      } else if (now - flameLastSeenAt >= 2 * FLAME_CLEAR_MS) {
        state = STATE_NORMAL;
        fireNotified = false;
        Serial.println("System back to NORMAL");
      }
      break;
  }
}

// ===================== Connectivity =====================
void maintainWifi(unsigned long now) {
  if (WiFi.status() == WL_CONNECTED) return;
  if (now - lastWifiAttempt < WIFI_RETRY_MS) return;
  lastWifiAttempt = now;
  Serial.println("Wi-Fi down — reconnecting...");
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
}

void publishStatus(unsigned long now) {
  if (now - lastStatusPush < STATUS_PUBLISH_MS) return;
  lastStatusPush = now;

  int flameIntensity = analogRead(PIN_FLAME_ANALOG);
  Serial.printf("[%s] mode=%s flame=%d pump=%s\n",
                stateName(), mode == MODE_AUTO ? "AUTO" : "MANUAL",
                flameIntensity, pumpOn ? "ON" : "OFF");

  if (Blynk.connected()) {
    Blynk.virtualWrite(VPIN_STATE, stateName());
    Blynk.virtualWrite(VPIN_FLAME_VALUE, flameIntensity);
    Blynk.virtualWrite(VPIN_PUMP_STATE, pumpOn ? 1 : 0);
  }
}

// ===================== Setup & Loop =====================
void setup() {
  Serial.begin(115200);

  pinMode(PIN_FLAME_DIGITAL, INPUT);
  pinMode(PIN_RELAY, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  pinMode(PIN_LED_RED, OUTPUT);
  pinMode(PIN_LED_GREEN, OUTPUT);

  setPump(false);
  setAlarm(false);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  // Non-blocking Blynk setup: config + connect with a short timeout,
  // so the safety logic starts even with no internet.
  Blynk.config(BLYNK_AUTH_TOKEN);
  Blynk.connect(5000);

  Serial.println("Fire detection system started");
}

void loop() {
  unsigned long now = millis();

  maintainWifi(now);
  if (Blynk.connected()) {
    Blynk.run();
  } else if (WiFi.status() == WL_CONNECTED) {
    Blynk.connect(0);   // quick non-blocking reconnect attempt
  }

  if (now - lastSensorPoll >= SENSOR_POLL_MS) {
    lastSensorPoll = now;
    bool flame = flameDetected();

    if (mode == MODE_AUTO) {
      runAutoLogic(now, flame);
    } else {
      // MANUAL mode: the app controls the pump, but we still alarm
      // locally so a fire is never silent.
      if (flame) {
        flameLastSeenAt = now;
        setAlarm(true);
      } else if (now - flameLastSeenAt >= FLAME_CLEAR_MS) {
        setAlarm(false);
      }
    }
  }

  publishStatus(now);
}
