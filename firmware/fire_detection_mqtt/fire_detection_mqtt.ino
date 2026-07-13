/*
 * IoT Fire Detection & Suppression System — ESP32 (MQTT version)
 * ---------------------------------------------------------------
 * Same local state machine as the Blynk version (NORMAL → FIRE →
 * COOLDOWN), but monitoring and remote control go over MQTT instead
 * of the Blynk cloud. Works with any MQTT broker: a local Mosquitto
 * on your LAN (recommended — no internet needed) or a cloud broker
 * like HiveMQ. Pair it with any MQTT dashboard app on your phone
 * (e.g. "IoT MQTT Panel") or Home Assistant / Node-RED.
 *
 * Topics (see config.h):
 *   Device publishes:  firesystem/state, /flame, /pump, /mode,
 *                      /alert, /availability (LWT)
 *   Device listens on: firesystem/cmd/pump, firesystem/cmd/mode
 *
 * Libraries (Arduino IDE → Library Manager):
 *   - PubSubClient (by Nick O'Leary)
 *
 * Before flashing: copy secrets.h.example → secrets.h and fill it in.
 */

#include "secrets.h"
#include "config.h"

#include <WiFi.h>
#include <PubSubClient.h>

WiFiClient wifiClient;
PubSubClient mqtt(wifiClient);

// ===================== System State =====================
enum SystemState { STATE_NORMAL, STATE_FIRE, STATE_COOLDOWN };
enum ControlMode { MODE_AUTO, MODE_MANUAL };

SystemState state = STATE_NORMAL;
ControlMode mode  = MODE_AUTO;

bool pumpOn            = false;
bool manualPumpCommand = false;   // last command received on cmd/pump

unsigned long flameFirstSeenAt = 0;
unsigned long flameLastSeenAt  = 0;
unsigned long lastSensorPoll   = 0;
unsigned long lastStatusPush   = 0;
unsigned long lastWifiAttempt  = 0;
unsigned long lastMqttAttempt  = 0;
bool fireNotified = false;

// ===================== Hardware Helpers =====================
void setPump(bool on) {
  pumpOn = on;
  digitalWrite(PIN_RELAY, (RELAY_ACTIVE_LOW ? !on : on) ? HIGH : LOW);
  if (mqtt.connected()) {
    mqtt.publish(TOPIC_PUMP, on ? "ON" : "OFF", true);
  }
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

// ===================== MQTT =====================
void onMqttMessage(char* topic, byte* payload, unsigned int length) {
  // Copy payload into a null-terminated buffer
  char msg[16];
  unsigned int n = min(length, (unsigned int)(sizeof(msg) - 1));
  memcpy(msg, payload, n);
  msg[n] = '\0';

  Serial.printf("MQTT <- %s: %s\n", topic, msg);

  if (strcmp(topic, TOPIC_CMD_PUMP) == 0) {
    manualPumpCommand = (strcasecmp(msg, "ON") == 0 || strcmp(msg, "1") == 0);
    if (mode == MODE_MANUAL) {
      setPump(manualPumpCommand);
    }
  } else if (strcmp(topic, TOPIC_CMD_MODE) == 0) {
    ControlMode newMode =
        (strcasecmp(msg, "MANUAL") == 0 || strcmp(msg, "1") == 0)
            ? MODE_MANUAL : MODE_AUTO;
    if (newMode != mode) {
      mode = newMode;
      Serial.printf("Mode changed to %s\n", mode == MODE_AUTO ? "AUTO" : "MANUAL");
      if (mode == MODE_MANUAL) {
        setPump(manualPumpCommand);
      }
      // Returning to AUTO: the state machine reasserts control on next loop
      if (mqtt.connected()) {
        mqtt.publish(TOPIC_MODE, mode == MODE_AUTO ? "AUTO" : "MANUAL", true);
      }
    }
  }
}

void maintainMqtt(unsigned long now) {
  if (WiFi.status() != WL_CONNECTED) return;
  if (mqtt.connected()) return;
  if (now - lastMqttAttempt < MQTT_RETRY_MS) return;
  lastMqttAttempt = now;

  Serial.println("Connecting to MQTT broker...");
  // Last Will: broker publishes "offline" if the device drops unexpectedly
  bool ok = mqtt.connect(MQTT_CLIENT_ID, MQTT_USER, MQTT_PASS,
                         TOPIC_AVAILABILITY, 1, true, "offline");
  if (ok) {
    Serial.println("MQTT connected");
    mqtt.publish(TOPIC_AVAILABILITY, "online", true);
    mqtt.publish(TOPIC_MODE, mode == MODE_AUTO ? "AUTO" : "MANUAL", true);
    mqtt.publish(TOPIC_PUMP, pumpOn ? "ON" : "OFF", true);
    mqtt.subscribe(TOPIC_CMD_PUMP);
    mqtt.subscribe(TOPIC_CMD_MODE);
  } else {
    Serial.printf("MQTT connect failed, rc=%d\n", mqtt.state());
  }
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
      if (flame && now - flameFirstSeenAt >= FLAME_CONFIRM_MS) {
        state = STATE_FIRE;
        setAlarm(true);
        setPump(true);
        Serial.println("!!! FIRE DETECTED — pump ON");
        if (mqtt.connected() && !fireNotified) {
          mqtt.publish(TOPIC_ALERT, "FIRE DETECTED - pump activated");
          fireNotified = true;
        }
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
  Serial.printf("[%s] mode=%s flame=%d pump=%s mqtt=%s\n",
                stateName(), mode == MODE_AUTO ? "AUTO" : "MANUAL",
                flameIntensity, pumpOn ? "ON" : "OFF",
                mqtt.connected() ? "up" : "down");

  if (mqtt.connected()) {
    char buf[8];
    snprintf(buf, sizeof(buf), "%d", flameIntensity);
    mqtt.publish(TOPIC_STATE, stateName(), true);
    mqtt.publish(TOPIC_FLAME, buf);
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

  mqtt.setServer(MQTT_BROKER, MQTT_PORT);
  mqtt.setCallback(onMqttMessage);

  Serial.println("Fire detection system (MQTT) started");
}

void loop() {
  unsigned long now = millis();

  maintainWifi(now);
  maintainMqtt(now);
  mqtt.loop();   // safe to call even when disconnected

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
