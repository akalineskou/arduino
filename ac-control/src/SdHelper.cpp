#include <SD.h>
#include <SPI.h>

#include "Directive.h"
#include "SdHelper.h"

SdHelper::SdHelper(const uint8_t pinSck, const uint8_t pinMiso, const uint8_t pinMosi, const uint8_t pinSs):
    pinSck(pinSck),
    pinMiso(pinMiso),
    pinMosi(pinMosi),
    pinSs(pinSs) {}

bool SdHelper::setup() const {
  SPI.begin(
    static_cast<int8_t>(pinSck),
    static_cast<int8_t>(pinMiso),
    static_cast<int8_t>(pinMosi),
    static_cast<int8_t>(pinSs)
  );

  if (!SD.begin(pinSs)) {
#if APP_DEBUG
    Serial.println("Card Mount Failed");
#endif

    return false;
  }

  if (SD.cardType() == CARD_NONE) {
#if APP_DEBUG
    Serial.println("No SD card attached");
#endif

    return false;
  }

  return true;
}

std::string SdHelper::readFile(const char* path) {
#if APP_DEBUG
  Serial.printf("Reading file: %s\n", path);
#endif

  File file = SD.open(path);
  if (!file) {
#if APP_DEBUG
    Serial.println("Failed to open file for reading");
#endif

    return "";
  }

  auto contents = std::string();

  while (file.available()) {
    contents += static_cast<char>(file.read());
  }

  file.close();

  return contents;
}
