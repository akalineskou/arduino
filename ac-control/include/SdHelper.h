#pragma once

#include <FS.h>

class SdHelper {
  uint8_t pinSck;
  uint8_t pinMiso;
  uint8_t pinMosi;
  uint8_t pinSs;

 public:
  explicit SdHelper(uint8_t pinSck, uint8_t pinMiso, uint8_t pinMosi, uint8_t pinSs);

  void setup() const;

  static std::string readFile(const char* path);
};
