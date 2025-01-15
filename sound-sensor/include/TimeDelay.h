#pragma once

class TimeDelay {
  unsigned long delay;
  bool repeat;
  bool forceFirstRun;

  unsigned long startTime = 0;
  bool finished = true;

 public:
  explicit TimeDelay(unsigned long delay);

  TimeDelay(unsigned long delay, bool repeat);

  bool delayPassed(bool force = false);

  void restart();

 private:
  void start();
};
