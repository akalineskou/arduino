#pragma once

#include <Arduino.h>

inline void stringReplace(std::string &string, const char* find, const char* replace) {
  while (string.find(find) != std::string::npos) {
    string.replace(string.find(find), std::string(find).length(), replace);
  }
}
