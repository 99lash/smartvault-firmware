#include <Arduino.h>
#include <Keypad.h>
#include "keypad.h"

static constexpr byte ROWS = 4;
static constexpr byte COLS = 4;

static char keymap[ROWS][COLS] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

static byte rowPins[ROWS] = {26, 25, 33, 32};
static byte colPins[COLS] = {13, 19, 14, 27};

static Keypad keypad = Keypad(makeKeymap(keymap), rowPins, colPins, ROWS, COLS);

void keypad_init() {
}

char keypad_get_key() {
  return keypad.getKey();
}
