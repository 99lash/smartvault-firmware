#pragma once
#include <Arduino.h>

bool nvs_storage_init();
bool nvs_storage_is_provisioned();
void nvs_storage_set_provisioned(bool provisioned);

void nvs_storage_save_wifi(const char* ssid, const char* password);
bool nvs_storage_load_wifi(char* ssid, size_t ssid_len, char* password, size_t password_len);

void nvs_storage_save_token(const char* token);
bool nvs_storage_load_token(char* token, size_t token_len);

void nvs_storage_save_api_url(const char* api_url);
bool nvs_storage_load_api_url(char* api_url, size_t api_url_len);

void nvs_storage_save_vault_id(const char* vault_id);
bool nvs_storage_load_vault_id(char* vault_id, size_t vault_id_len);

void nvs_storage_save_nfc_uid(const char* uid);
bool nvs_storage_load_nfc_uid(char* uid, size_t uid_len);

void nvs_storage_erase_all();
