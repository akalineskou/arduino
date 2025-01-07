#pragma once

class TimeDelay {
  unsigned long delay;
  unsigned long startTime;

 public:
  explicit TimeDelay(unsigned long delay);

  bool finished(bool finishNow = false, bool restart = false);
};
