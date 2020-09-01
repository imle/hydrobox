#include <debug.h>

#include <Arduino.h>

volatile int indent = 0;

void printSpaces(bool minus_one) {
  for (int i = 0; i < indent - (int)minus_one; ++i) {
    Serial.print(" ");
  }
}
