#include <driver/i2s.h>

#include "Directive.h"
#include "MicrophoneHelper.h"

MicrophoneHelper::MicrophoneHelper(const int i2sClockPin, const int i2sWsPin, const int i2sDataInPin):
    i2sClockPin(i2sClockPin),
    i2sWsPin(i2sWsPin),
    i2sDataInPin(i2sDataInPin),
    FFT(ArduinoFFT<float>(vReal, vImag, SAMPLE_SIZE, 44100)) {}

void MicrophoneHelper::setup() const {
  constexpr i2s_driver_config_t i2sConfig = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 44100,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = SAMPLE_SIZE,
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

void MicrophoneHelper::getBands(Bands &bands) {
  size_t bytesIn = 0;
  const esp_err_t result = i2s_read(I2S_NUM_0, rawData, SAMPLE_SIZE * sizeof(int32_t), &bytesIn, portMAX_DELAY);
  if (result != ESP_OK) {
#if APP_DEBUG
    Serial.println("I2S read failed");
#endif
  }

  for (int i = 0; i < SAMPLE_SIZE; i++) {
    vReal[i] = static_cast<float>(rawData[i]);
    vImag[i] = 0;
  }

  FFT.dcRemoval();
  FFT.windowing(FFTWindow::Hamming, FFTDirection::Forward, false);
  FFT.compute(FFTDirection::Forward);
  FFT.complexToMagnitude();

  for (auto i = 0; i < BAND_SIZE; i++) {
    bands.data[i] = static_cast<int>(vReal[i] / 10000000);
  }
}
