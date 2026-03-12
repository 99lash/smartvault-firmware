#pragma once
#include <Arduino.h>

struct ProvisioningData {
  char ssid[64];
  char password[64];
  char token[128];
  char api_url[128];
};

typedef void (*provisioning_done_cb_t)(const ProvisioningData &data);

void provisioning_start(provisioning_done_cb_t callback);
void provisioning_stop();
void provisioning_loop();
bool provisioning_is_active();
