#include <Arduino.h>

#include "Led.h"

Led::Led(const int pin): pin(pin) {}

void Led::setup() const {
  pinMode(pin, OUTPUT);
}

void Led::on() const {
  digitalWrite(pin, HIGH);
}

void Led::off() const {
  digitalWrite(pin, LOW);
}
