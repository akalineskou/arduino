#pragma once

class ButtonEnabled {
  int pin;

  unsigned long lastCall;

public:
  bool enabled;
  bool hasChanged;

  explicit ButtonEnabled(int pin);

  void setup() const;

  void loop();

private:
  void check();
};
