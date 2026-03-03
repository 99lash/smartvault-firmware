#pragma once
#include <Arduino.h>
#include "pin_auth.h"

void pin_entry_init(pin_auth_result_cb_t callback);
void pin_entry_handle_key(char key); // call this with each keypad key
