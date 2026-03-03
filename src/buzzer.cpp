#include "buzzer.h"

namespace {
void beep(unsigned long duration_ms) {
  digitalWrite(BUZZER_PIN, HIGH);
  delay(duration_ms);
  digitalWrite(BUZZER_PIN, LOW);
}
}

void buzzer_init() {
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void buzzer_keypress() {
  beep(50);
}

void buzzer_success() {
  beep(100);
  delay(100);
  beep(100);
}

void buzzer_fail() {
  beep(500);
}

void buzzer_tamper() {
  for (int i = 0; i < 3; i++) {
    beep(100);
    delay(100);
  }
}
