#pragma once

#include <arduinoFFT.h>
#include <driver/i2s.h>

#include "Bands.h"

class MicrophoneHelper {
  int i2sClockPin;
  int i2sWsPin;
  int i2sDataInPin;

  uint32_t sampleRate;
  i2s_port_t i2sPort;
  ArduinoFFT<double> FFT;
  double vReal[SAMPLES]{};
  double vImag[SAMPLES]{};
  int32_t rawData[SAMPLES]{};

 public:
  explicit MicrophoneHelper(int i2sClockPin, int i2sWsPin, int i2sDataInPin);

  void setup() const;

  void getBands(Bands &bands);
};
