#include <Preferences.h>
#include <Arduino.h>
#include "nvs_storage.h"

namespace {
const char kNamespace[] = "smrtvlt";
const char kKeyProvisioned[] = "provisioned";
const char kKeyWifiSsid[] = "wifi_ssid";
const char kKeyWifiPass[] = "wifi_pass";
const char kKeyToken[] = "token";
const char kKeyApiUrl[] = "api_url";
const char kKeyNfcUid[] = "nfc_uid";

Preferences g_preferences;
bool g_initialized = false;

bool nvs_storage_load_string(const char* key, char* value, size_t value_len) {
  if (!value || value_len == 0) {
    return false;
  }

  size_t read_len = g_preferences.getString(key, value, value_len);
  if (read_len == 0) {
    value[0] = '\0';
    Serial.print("NVS load missing: ");
    Serial.println(key);
    return false;
  }

  Serial.print("NVS loaded: ");
  Serial.println(key);
  return true;
}
}  // namespace

bool nvs_storage_init() {
  if (g_initialized) {
    return true;
  }

  bool started = g_preferences.begin(kNamespace, false);
  g_initialized = started;
  if (started) {
    Serial.println("NVS initialized");
  } else {
    Serial.println("NVS init failed");
  }
  return started;
}

bool nvs_storage_is_provisioned() {
  bool provisioned = g_preferences.getBool(kKeyProvisioned, false);
  Serial.print("NVS provisioned: ");
  Serial.println(provisioned ? "true" : "false");
  return provisioned;
}

void nvs_storage_set_provisioned(bool provisioned) {
  g_preferences.putBool(kKeyProvisioned, provisioned);
  Serial.print("NVS set provisioned: ");
  Serial.println(provisioned ? "true" : "false");
}

void nvs_storage_save_wifi(const char* ssid, const char* password) {
  g_preferences.putString(kKeyWifiSsid, ssid ? ssid : "");
  g_preferences.putString(kKeyWifiPass, password ? password : "");
  Serial.println("NVS saved WiFi");
}

bool nvs_storage_load_wifi(char* ssid, size_t ssid_len, char* password, size_t password_len) {
  bool ssid_ok = nvs_storage_load_string(kKeyWifiSsid, ssid, ssid_len);
  bool pass_ok = nvs_storage_load_string(kKeyWifiPass, password, password_len);
  if (!ssid_ok || !pass_ok) {
    Serial.println("NVS WiFi load failed");
    return false;
  }

  Serial.println("NVS WiFi loaded");
  return true;
}

void nvs_storage_save_token(const char* token) {
  g_preferences.putString(kKeyToken, token ? token : "");
  Serial.println("NVS saved token");
}

bool nvs_storage_load_token(char* token, size_t token_len) {
  bool ok = nvs_storage_load_string(kKeyToken, token, token_len);
  if (!ok) {
    Serial.println("NVS token load failed");
  }
  return ok;
}

void nvs_storage_save_api_url(const char* api_url) {
  g_preferences.putString(kKeyApiUrl, api_url ? api_url : "");
  Serial.println("NVS saved API URL");
}

bool nvs_storage_load_api_url(char* api_url, size_t api_url_len) {
  bool ok = nvs_storage_load_string(kKeyApiUrl, api_url, api_url_len);
  if (!ok) {
    Serial.println("NVS API URL load failed");
  }
  return ok;
}

void nvs_storage_save_nfc_uid(const char* uid) {
  g_preferences.putString(kKeyNfcUid, uid ? uid : "");
  Serial.println("NVS saved NFC UID");
}

bool nvs_storage_load_nfc_uid(char* uid, size_t uid_len) {
  bool ok = nvs_storage_load_string(kKeyNfcUid, uid, uid_len);
  if (!ok) {
    Serial.println("NVS NFC UID load failed");
  }
  return ok;
}

void nvs_storage_erase_all() {
  g_preferences.clear();
  Serial.println("NVS cleared");
}
