#pragma once

class InfraredReceiver {
  int pin;

 public:
  explicit InfraredReceiver(int pin);

  void setup();

  void loop();
};
