#include <Arduino.h>

#include "Directive.h"
#include "TimeHelper.h"

TimeHelper::TimeHelper(): timeInfo({}) {
  currentTime = 0;
}

void TimeHelper::setup() {
  configTime(0, 0, "pool.ntp.org");

  setTime();

#if APP_DEBUG
  Serial.printf("Current UTC time: %d (%s)\n", currentTime, formatForHuman(currentTime).c_str());
#endif

  yield();
}

void TimeHelper::loop() {
  setTime();
}

std::string TimeHelper::formatForCode(const time_t time) {
  return format(time, "%Y-%m-%d %H:%M:%S");
}

std::string TimeHelper::formatForHuman(const time_t time) {
  return format(time, "%d-%b-%Y %H:%M:%S");
}

void TimeHelper::setTime() {
  if (!getLocalTime(&timeInfo)) {
#if APP_DEBUG
    Serial.println("Failed to obtain UTC time");
#endif
    return;
  }

  time(&currentTime);

  setenv("TZ", "UTC0", 1);
  tzset();
}

std::string TimeHelper::format(const time_t time, const char* format) {
  setenv("TZ", "EET-2EEST,M3.5.0/3,M10.5.0/4", 1);
  tzset();

  tm timeInfo{};
  localtime_r(&time, &timeInfo);

  setenv("TZ", "UTC0", 1);
  tzset();

  char buf[64];
  strftime(buf, 64, format, &timeInfo);

  return {buf};
}
