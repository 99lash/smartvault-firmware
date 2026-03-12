#pragma once
#include <Arduino.h>

void api_client_init(const char* base_url);
bool api_client_register_device(
    const char* hardware_uuid,
    const char* token,
    char* vault_id_out,
    size_t vault_id_len
);
bool api_client_send_tamper_alert(const char* hardware_uuid);
bool api_client_deprovision_device(const char* hardware_uuid);
