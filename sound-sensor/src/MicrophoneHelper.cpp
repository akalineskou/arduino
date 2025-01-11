#include "Directive.h"
#include "MicrophoneHelper.h"

MicrophoneHelper::MicrophoneHelper(const int i2sClockPin, const int i2sWsPin, const int i2sDataInPin):
    i2sClockPin(i2sClockPin),
    i2sWsPin(i2sWsPin),
    i2sDataInPin(i2sDataInPin),
    sampleRate(8500),
    i2sPort(I2S_NUM_0),
    FFT(ArduinoFFT<double>(vReal, vImag, BLOCK_SIZE, sampleRate)) {}

void MicrophoneHelper::setup() const {
#if DEBUG
  Serial.println("Configuring I2S...");
#endif

  const i2s_driver_config_t i2sConfig = {
    .mode = static_cast<i2s_mode_t>(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = sampleRate,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_STAND_I2S,
    .intr_alloc_flags = 0,
    .dma_buf_count = 8,
    .dma_buf_len = BLOCK_SIZE_HALF,
    .use_apll = false,
  };

  // Configuring the I2S driver and pins.
  // This function must be called before any I2S driver read/write operations.
  esp_err_t err = i2s_driver_install(i2sPort, &i2sConfig, 0, nullptr);
  if (err != ESP_OK) {
#if DEBUG
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

  err = i2s_set_pin(i2sPort, &pinConfig);
  if (err != ESP_OK) {
#if DEBUG
    Serial.printf("Failed setting pin: %d\n", err);
#endif
    while (true);
  }

#if DEBUG
  Serial.println("I2S driver installed.");
#endif
}

void MicrophoneHelper::getBands(Bands &bands) {
  // Read multiple samples at once and calculate the sound pressure
  size_t bytesIn = 0;
  const esp_err_t result = i2s_read(i2sPort, &samples, BLOCK_SIZE, &bytesIn, portMAX_DELAY);
  if (result != ESP_OK) {
#if DEBUG
    Serial.println("I2S read failed");
#endif
    return;
  }

  for (uint16_t i = 0; i < BLOCK_SIZE; i++) {
    vReal[i] = samples[i];
    vImag[i] = 0.0;
  }

  FFT.windowing(vReal, BLOCK_SIZE, FFTWindow::Rectangle, FFTDirection::Forward);
  FFT.compute(vReal, vImag, BLOCK_SIZE, FFTDirection::Forward);
  FFT.complexToMagnitude(vReal, vImag, BLOCK_SIZE);

  auto j = 0;

  // Don't use sample 0 and only first SAMPLES/2 are usable.
  for (int i = 2; i < BLOCK_SIZE_HALF; i++) {
    bands.data[j] = static_cast<int>(vReal[i] / 1000000);

    // ignore as noise
    if (bands.data[j] < 500) {
      bands.data[j] = 0;
    }

    j++;
  }
}
