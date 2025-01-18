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

  static std::string getDateTimeFormatted(time_t time);

 private:
  void setTime();
};
