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
  Serial.printf("Current UTC time: %d\n", currentTime);
#endif

  yield();
}

void TimeHelper::loop() {
  setTime();
}

void TimeHelper::setTime() {
  if (!getLocalTime(&timeInfo)) {
#if APP_DEBUG
    Serial.println("Failed to obtain UTC time");
#endif
    return;
  }

  time(&currentTime);
}

std::string TimeHelper::getDateTimeFormatted(const time_t time) {
  tm timeInfo{};
  localtime_r(&time, &timeInfo);

  char buf[64];
  strftime(buf, 64, "%d-%b-%Y %H:%M:%S", &timeInfo);

  return {buf};
}
