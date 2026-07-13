#include "wifi_manager.h"

#include <WiFi.h>

#include "config.h"
#include "constants.h"

namespace WifiManager {

static unsigned long s_lastAttempt = 0;
static bool s_wasConnected = false;

void begin() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.printf("Wi-Fi: connecting to \"%s\"...\n", WIFI_SSID);
}

void loop() {
  bool connected = (WiFi.status() == WL_CONNECTED);

  // Report the moment the link comes up (prints the IP address).
  if (connected && !s_wasConnected) {
    Serial.print("Wi-Fi: connected. IP address: ");
    Serial.println(WiFi.localIP());
  } else if (!connected && s_wasConnected) {
    Serial.println("Wi-Fi: disconnected");
  }
  s_wasConnected = connected;

  // Periodic non-blocking reconnect attempts.
  if (!connected) {
    unsigned long now = millis();
    if (now - s_lastAttempt >= WIFI_RETRY_MS) {
      s_lastAttempt = now;
      Serial.println("Wi-Fi: retrying...");
      WiFi.disconnect();
      WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    }
  }
}

bool isConnected() { return WiFi.status() == WL_CONNECTED; }

String ipAddress() { return WiFi.localIP().toString(); }

}  // namespace WifiManager
