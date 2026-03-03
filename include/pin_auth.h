#pragma once
#include <Arduino.h>

typedef void (*pin_auth_result_cb_t)(bool accepted);

void pin_auth_init(const char* base_url, const char* hardware_uuid);
void pin_auth_verify(const char* pin, pin_auth_result_cb_t callback);
