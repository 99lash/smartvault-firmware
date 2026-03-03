#include <Arduino.h>
#include <WiFi.h>

#include "wifi_manager.h"

bool wifi_manager_connect(const char* ssid, const char* password) {
  Serial.print("WiFi connecting to: ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  for (int attempt = 1; attempt <= 5; ++attempt) {
    if (WiFi.status() == WL_CONNECTED) {
      Serial.println("WiFi connected");
      Serial.print("IP: ");
      Serial.println(WiFi.localIP());
      return true;
    }
    Serial.print("WiFi retry ");
    Serial.println(attempt);
    delay(2000);
  }

  Serial.println("WiFi connection failed");
  return false;
}

bool wifi_manager_is_connected() {
  return WiFi.status() == WL_CONNECTED;
}

void wifi_manager_disconnect() {
  Serial.println("WiFi disconnect");
  WiFi.disconnect();
}
