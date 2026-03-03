#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>

#include "provisioning.h"

namespace {
const IPAddress kApIp(192, 168, 4, 1);
const IPAddress kApNetmask(255, 255, 255, 0);

DNSServer dns_server;
WebServer web_server(80);
provisioning_done_cb_t done_callback = nullptr;
bool provisioning_active = false;
bool stop_pending = false;

const char kProvisioningPage[] PROGMEM =
    "<!DOCTYPE html>"
    "<html lang='en'>"
    "<head>"
    "<meta charset='utf-8'>"
    "<meta name='viewport' content='width=device-width, initial-scale=1'>"
    "<title>SmartVault Provisioning</title>"
    "<style>"
    "body{font-family:Arial,Helvetica,sans-serif;margin:0;padding:24px;background:#f5f6f8;color:#111;}"
    ".card{max-width:420px;margin:0 auto;background:#fff;border-radius:10px;padding:24px;"
    "box-shadow:0 4px 14px rgba(0,0,0,0.08);}"
    "h1{font-size:20px;margin:0 0 16px 0;}"
    "label{display:block;font-size:13px;margin:12px 0 6px 0;color:#333;}"
    "input{width:100%;padding:10px 12px;border:1px solid #cfd4db;border-radius:6px;"
    "font-size:14px;box-sizing:border-box;}"
    "button{margin-top:18px;width:100%;padding:12px;background:#1e88e5;border:none;"
    "border-radius:6px;color:#fff;font-size:15px;cursor:pointer;}"
    "button:active{transform:translateY(1px);}"
    "</style>"
    "</head>"
    "<body>"
    "<div class='card'>"
    "<h1>SmartVault Provisioning</h1>"
    "<form method='POST' action='/provision'>"
    "<label for='ssid'>WiFi SSID</label>"
    "<input id='ssid' name='ssid' type='text' required>"
    "<label for='password'>WiFi Password</label>"
    "<input id='password' name='password' type='password' required>"
    "<label for='token'>Token</label>"
    "<input id='token' name='token' type='text' required>"
    "<label for='api_url'>API URL</label>"
    "<input id='api_url' name='api_url' type='text' required>"
    "<button type='submit'>Provision</button>"
    "</form>"
    "</div>"
    "</body>"
    "</html>";

String build_ap_ssid() {
  String mac = WiFi.macAddress();
  String suffix = mac.length() >= 17 ? mac.substring(9) : mac;
  suffix.replace(":", "");
  suffix.toUpperCase();
  return "SmartVault-" + suffix;
}

void copy_field(const String &value, char *dest, size_t size) {
  if (size == 0) {
    return;
  }
  size_t copy_len = value.length();
  if (copy_len >= size) {
    copy_len = size - 1;
  }
  memcpy(dest, value.c_str(), copy_len);
  dest[copy_len] = '\0';
}

void handle_root() {
  web_server.send(200, "text/html", kProvisioningPage);
}

void handle_provision() {
  ProvisioningData data = {};
  copy_field(web_server.arg("ssid"), data.ssid, sizeof(data.ssid));
  copy_field(web_server.arg("password"), data.password, sizeof(data.password));
  copy_field(web_server.arg("token"), data.token, sizeof(data.token));
  copy_field(web_server.arg("api_url"), data.api_url, sizeof(data.api_url));

  web_server.send(200, "text/html",
                  "<html><body><h2>Provisioning received.</h2>"
                  "<p>You can return to the app.</p></body></html>");

  if (done_callback) {
    done_callback(data);
  }

  stop_pending = true;
}

void handle_not_found() {
  web_server.sendHeader("Location", "/", true);
  web_server.send(302, "text/plain", "");
}
}  // namespace

void provisioning_start(provisioning_done_cb_t callback) {
  if (provisioning_active) {
    return;
  }

  done_callback = callback;
  stop_pending = false;

  WiFi.mode(WIFI_AP);
  String ssid = build_ap_ssid();
  WiFi.softAPConfig(kApIp, kApIp, kApNetmask);
  WiFi.softAP(ssid.c_str());

  dns_server.start(53, "*", kApIp);

  web_server.on("/", HTTP_GET, handle_root);
  web_server.on("/provision", HTTP_POST, handle_provision);
  web_server.onNotFound(handle_not_found);
  web_server.begin();

  provisioning_active = true;
}

void provisioning_stop() {
  if (!provisioning_active) {
    return;
  }

  dns_server.stop();
  web_server.stop();
  WiFi.softAPdisconnect(true);
  WiFi.mode(WIFI_STA);

  provisioning_active = false;
  stop_pending = false;
}

void provisioning_loop() {
  if (!provisioning_active) {
    return;
  }

  dns_server.processNextRequest();
  web_server.handleClient();

  if (stop_pending) {
    provisioning_stop();
  }
}

bool provisioning_is_active() {
  return provisioning_active;
}
