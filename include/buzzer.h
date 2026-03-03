#pragma once
#include <Arduino.h>

#define BUZZER_PIN 5

void buzzer_init();
void buzzer_keypress();   // 1 short beep (50ms)
void buzzer_success();    // 2 short beeps
void buzzer_fail();       // 1 long beep (500ms)
void buzzer_tamper();     // 3 rapid beeps
