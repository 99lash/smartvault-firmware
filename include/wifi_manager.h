#pragma once
#include <Arduino.h>

bool wifi_manager_connect(const char* ssid, const char* password);
bool wifi_manager_is_connected();
void wifi_manager_disconnect();
