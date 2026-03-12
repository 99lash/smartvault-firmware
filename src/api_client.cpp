#include <Arduino.h>
#include <HTTPClient.h>
#include <WiFiClient.h>
#include <WiFiClientSecure.h>
#include "api_client.h"

namespace {
constexpr size_t kBaseUrlSize = 256;
constexpr size_t kUrlSize = 320;
constexpr size_t kBodySize = 256;
constexpr int kMaxRetries = 3;
constexpr int kRetryDelayMs = 2000;

char g_base_url[kBaseUrlSize] = {0};

bool is_https() {
  return strncmp(g_base_url, "https://", 8) == 0;
}
}  // namespace

void api_client_init(const char* base_url) {
  if (!base_url) {
    g_base_url[0] = '\0';
    return;
  }
  strncpy(g_base_url, base_url, kBaseUrlSize - 1);
  g_base_url[kBaseUrlSize - 1] = '\0';
}

bool api_client_register_device(
    const char* hardware_uuid,
    const char* token,
    char* vault_id_out,
    size_t vault_id_len
) {
  if (g_base_url[0] == '\0') {
    Serial.println("API client not initialized");
    return false;
  }
  if (!hardware_uuid || !token) {
    Serial.println("Missing hardware UUID or token");
    return false;
  }

  char url[kUrlSize];
  snprintf(url, sizeof(url), "%s/api/v1/devices/register", g_base_url);

  char body[kBodySize];
  snprintf(body, sizeof(body),
           "{\"hardware_uuid\":\"%s\",\"provisioning_token\":\"%s\"}",
           hardware_uuid, token);

  for (int attempt = 1; attempt <= kMaxRetries; ++attempt) {
    WiFiClientSecure secure_client;
    WiFiClient plain_client;
    HTTPClient http;

    if (is_https()) {
      secure_client.setInsecure();
      http.begin(secure_client, url);
    } else {
      http.begin(plain_client, url);
    }
    http.addHeader("Content-Type", "application/json");
    int status = http.POST(reinterpret_cast<uint8_t*>(body), strlen(body));
    Serial.print("Register attempt ");
    Serial.print(attempt);
    Serial.print(" status: ");
    Serial.println(status);

    if (status == 200) {
      String response = http.getString();
      http.end();

      Serial.print("Registration response: ");
      Serial.println(response);

      int vault_id_start = response.indexOf("\"vault_id\":\"") + 12;
      int vault_id_end = response.indexOf("\"", vault_id_start);
      if (vault_id_start > 12 && vault_id_end > vault_id_start && vault_id_out && vault_id_len > 0) {
        String vault_id = response.substring(vault_id_start, vault_id_end);
        strncpy(vault_id_out, vault_id.c_str(), vault_id_len - 1);
        vault_id_out[vault_id_len - 1] = '\0';
        Serial.print("Extracted vault_id: ");
        Serial.println(vault_id_out);
      }
      return true;
    }

    if (status == 409) {
      Serial.println("Device already registered (409)");
      Serial.println("Attempting backend deprovision");
      http.end();

      bool deprovisioned = api_client_deprovision_device(hardware_uuid);
      if (deprovisioned) {
        Serial.println("Deprovisioned old vault, retrying register");
        delay(1000);
        continue;
      }
      Serial.println("Deprovision failed, cannot register");
      return false;
    }

    http.end();

    if (status == 401) {
      Serial.println("Invalid provisioning token (401)");
      return false;
    }

    if (attempt < kMaxRetries) {
      delay(kRetryDelayMs);
    }
  }

  return false;
}

bool api_client_send_tamper_alert(const char* hardware_uuid) {
  if (g_base_url[0] == '\0') {
    Serial.println("API client not initialized");
    return false;
  }
  if (!hardware_uuid) {
    Serial.println("Missing hardware UUID");
    return false;
  }

  char url[kUrlSize];
  snprintf(url, sizeof(url), "%s/api/v1/devices/tamper", g_base_url);

  char body[kBodySize];
  snprintf(body, sizeof(body), "{\"hardware_uuid\":\"%s\"}", hardware_uuid);

  WiFiClientSecure secure_client;
  WiFiClient plain_client;
  HTTPClient http;

  if (is_https()) {
    secure_client.setInsecure();
    http.begin(secure_client, url);
  } else {
    http.begin(plain_client, url);
  }
  http.addHeader("Content-Type", "application/json");
  int status = http.POST(reinterpret_cast<uint8_t*>(body), strlen(body));
  Serial.print("Tamper alert status: ");
  Serial.println(status);
  http.end();

  return status == 200;
}

bool api_client_deprovision_device(const char* hardware_uuid) {
  if (g_base_url[0] == '\0') {
    Serial.println("API client not initialized");
    return false;
  }
  if (!hardware_uuid) {
    Serial.println("Missing hardware UUID");
    return false;
  }

  char url[kUrlSize];
  snprintf(url, sizeof(url), "%s/api/v1/devices/deprovision", g_base_url);

  char body[kBodySize];
  snprintf(body, sizeof(body), "{\"hardware_uuid\":\"%s\"}", hardware_uuid);

  Serial.println("Attempting backend deprovision");

  for (int attempt = 1; attempt <= kMaxRetries; ++attempt) {
    WiFiClientSecure secure_client;
    WiFiClient plain_client;
    HTTPClient http;

    if (is_https()) {
      secure_client.setInsecure();
      http.begin(secure_client, url);
    } else {
      http.begin(plain_client, url);
    }
    http.addHeader("Content-Type", "application/json");
    int status = http.POST(reinterpret_cast<uint8_t*>(body), strlen(body));
    Serial.print("Deprovision attempt ");
    Serial.print(attempt);
    Serial.print(" status: ");
    Serial.println(status);
    http.end();

    if (status == 200) {
      Serial.println("Deprovisioned device");
      return true;
    }
    if (status == 404) {
      Serial.println("Device not registered");
      return true;
    }
    if (attempt < kMaxRetries) {
      delay(5000);
    }
  }

  Serial.println("Deprovision failed after retries");
  return false;
}
