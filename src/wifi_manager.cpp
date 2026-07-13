#include <Arduino.h>
#include <WiFi.h>
#include "config.h"
#include "constants.h"
#include "wifi_manager.h"

bool wifiConnect() {
  Serial.print("[WIFI] Connecting to \"");
  Serial.print(WIFI_SSID);
  Serial.print("\" ");

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  unsigned long startTime = millis();
  unsigned long lastDotTime = 0;

  // Wait for the connection, but never longer than the timeout.
  while (WiFi.status() != WL_CONNECTED) {
    unsigned long now = millis();

    if (now - startTime >= WIFI_CONNECT_TIMEOUT_MS) {
      Serial.println();
      Serial.println("[WIFI] FAILED - continuing in OFFLINE mode.");
      return false;
    }

    // Progress feedback without delay(): print a dot every 500 ms.
    if (now - lastDotTime >= WIFI_PROGRESS_INTERVAL_MS) {
      lastDotTime = now;
      Serial.print(".");
    }
  }

  Serial.println();
  wifiPrintStatus();
  return true;
}

bool wifiIsConnected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifiPrintStatus() {
  if (wifiIsConnected()) {
    Serial.println("[WIFI] Status: CONNECTED");
    Serial.print("[WIFI] IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("[WIFI] Status: DISCONNECTED");
  }
}
