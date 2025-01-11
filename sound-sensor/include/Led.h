#pragma once

class Led {
 public:
  int pin;

  explicit Led(int pin);

  void setup() const;

  void on() const;

  void off() const;
};
