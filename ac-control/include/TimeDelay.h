#pragma once

class TimeDelay {
  unsigned long delay;
  bool repeat;
  bool forceFirstRun;
  unsigned long startTime = 0;
  bool running = false;

 public:
  explicit TimeDelay(unsigned long delay, bool repeat);

  bool delayPassed(bool force = false);

 private:
  void start();
};
