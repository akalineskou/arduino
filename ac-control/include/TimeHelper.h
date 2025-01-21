#pragma once

#include <ctime>
#include <string>

class TimeHelper {
 public:
  time_t currentTime;
  tm timeInfo;

  TimeHelper();

  void setup();

  void loop();

  static std::string formatForCode(time_t time);

  static std::string formatForHuman(time_t time);

 private:
  void setTime();

  static std::string format(time_t time, const char* format);
};
