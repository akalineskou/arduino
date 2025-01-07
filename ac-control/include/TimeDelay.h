#pragma once

class TimeDelay {
  unsigned long delay;
  unsigned long startTime = -1;
  unsigned long running = false;

 public:
  explicit TimeDelay(unsigned long delay);

  bool finished(bool force = false, bool restart = false);

 private:
  void start();
};
