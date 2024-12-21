#pragma once

class ButtonEnabled {
  int pin;

  unsigned long lastCall = 0;

public:
  bool enabled = false;
  bool hasChanged = false;

  explicit ButtonEnabled(const int pin): pin(pin) {
  }

  void setup() const;

  void loop();
};
