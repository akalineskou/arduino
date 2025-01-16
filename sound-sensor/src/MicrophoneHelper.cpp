#include <driver/i2s.h>

#include "Directive.h"
#include "MicrophoneHelper.h"

MicrophoneHelper::MicrophoneHelper(const int i2sClockPin, const int i2sWsPin, const int i2sDataInPin):
    i2sClockPin(i2sClockPin),
    i2sWsPin(i2sWsPin),
    i2sDataInPin(i2sDataInPin),
    idleTimeDelay(100, true) {
  previousIdling = false;
  sampleRate = 44100;
  sampleSize = 0;
  i2sBitsPerSample = I2S_BITS_PER_SAMPLE_32BIT;
  i2sReadSize = 0;
  bandSize = 0;
  FFT = nullptr;
}

void MicrophoneHelper::getBands(Bands &bands, const bool idling) {
  if (nullptr == FFT || previousIdling != idling) {
    if (FFT != nullptr) {
      // uninstall previous driver
      i2s_driver_uninstall(I2S_NUM_0);
    }

    // update value
    previousIdling = idling;

    if (!idling) {
      sampleSize = SAMPLE_SIZE;
      bandSize = BAND_SIZE;
      i2sBitsPerSample = I2S_BITS_PER_SAMPLE_32BIT;
      i2sReadSize = sampleSize * sizeof(int32_t);
    } else {
      sampleSize = BAND_SIZE; // min band size
      bandSize = BAND_SIZE / 2;
      i2sBitsPerSample = I2S_BITS_PER_SAMPLE_8BIT;
      i2sReadSize = sampleSize;
    }

    FFT = new ArduinoFFT<float>(vReal, vImag, sampleSize, static_cast<float>(sampleRate));

    if (idling) {
      idleTimeDelay.restart();
    }

    const i2s_driver_config_t i2sConfig = {
      .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = sampleRate,
      .bits_per_sample = i2sBitsPerSample,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_STAND_I2S,
      .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
      .dma_buf_count = 8,
      .dma_buf_len = sampleSize,
      .use_apll = false,
      .tx_desc_auto_clear = false,
      .fixed_mclk = 0,
    };

    // Configuring the I2S driver and pins.
    // This function must be called before any I2S driver read/write operations.
    esp_err_t err = i2s_driver_install(I2S_NUM_0, &i2sConfig, 0, nullptr);
    if (err != ESP_OK) {
#if APP_DEBUG
      Serial.printf("Failed installing driver: %d\n", err);
#endif
      while (true);
    }

    // The pin config as per the setup
    const i2s_pin_config_t pinConfig = {
      .bck_io_num = i2sClockPin,
      .ws_io_num = i2sWsPin,
      .data_out_num = -1,
      .data_in_num = i2sDataInPin,
    };

    err = i2s_set_pin(I2S_NUM_0, &pinConfig);
    if (err != ESP_OK) {
#if APP_DEBUG
      Serial.printf("Failed setting pin: %d\n", err);
#endif
      while (true);
    }

    err = i2s_zero_dma_buffer(I2S_NUM_0);
    if (err != ESP_OK) {
#if APP_DEBUG
      Serial.printf("Failed setting zero dma buffer: %d\n", err);
#endif
      while (true);
    }

#if APP_DEBUG
    Serial.println("I2S driver installed.");
#endif
  }

  if (previousIdling && !idleTimeDelay.delayPassed()) {
    return;
  }

  size_t bytesIn = 0;
  const esp_err_t result = i2s_read(I2S_NUM_0, rawData, i2sReadSize, &bytesIn, portMAX_DELAY);
  if (result != ESP_OK) {
#if APP_DEBUG
    Serial.println("I2S read failed");
#endif
  }

  for (int i = 0; i < sampleSize; i++) {
    vReal[i] = static_cast<float>(rawData[i]);
    vImag[i] = 0;
  }

  FFT->dcRemoval();
  FFT->windowing(FFTWindow::Hamming, FFTDirection::Forward, false);
  FFT->compute(FFTDirection::Forward);
  FFT->complexToMagnitude();

  for (auto i = 0; i < bandSize; i++) {
    bands.data[i] = static_cast<int>(vReal[i] / 10000000);

    if (bands.data[i] < 50) {
      // mark as noise
      bands.data[i] = 0;
    }
  }
}
