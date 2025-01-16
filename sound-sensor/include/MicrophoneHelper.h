#pragma once

#include <TimeDelay.h>
#include <arduinoFFT.h>
#include <driver/i2s.h>

#include "Bands.h"

#define SAMPLE_SIZE 1024

class MicrophoneHelper {
  int i2sClockPin;
  int i2sWsPin;
  int i2sDataInPin;

  bool previousIdling;
  uint32_t sampleRate;
  int sampleSize;
  size_t i2sReadSize;
  i2s_bits_per_sample_t i2sBitsPerSample;
  int bandSize;
  float vReal[SAMPLE_SIZE]{};
  float vImag[SAMPLE_SIZE]{};
  ArduinoFFT<float>* FFT;
  int32_t rawData[SAMPLE_SIZE]{};
  TimeDelay idleTimeDelay;

 public:
  explicit MicrophoneHelper(int i2sClockPin, int i2sWsPin, int i2sDataInPin);

  void getBands(Bands &bands, bool idling);
};
