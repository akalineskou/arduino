#pragma once

#include <arduinoFFT.h>

#include "Bands.h"

#define SAMPLE_SIZE 1024

class MicrophoneHelper {
  int i2sClockPin;
  int i2sWsPin;
  int i2sDataInPin;

  float vReal[SAMPLE_SIZE]{};
  float vImag[SAMPLE_SIZE]{};
  ArduinoFFT<float> FFT;
  int32_t rawData[SAMPLE_SIZE]{};

 public:
  explicit MicrophoneHelper(int i2sClockPin, int i2sWsPin, int i2sDataInPin);

  void setup() const;

  void getBands(Bands &bands);
};
