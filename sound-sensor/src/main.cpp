#include <Arduino.h>

#define DEBUG 1
#define PLOT 0

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__);
#define D_print(...)  Serial.print(__VA_ARGS__)
#define D_println(...)  Serial.println(__VA_ARGS__)
#define D_printf(...)  Serial.printf(__VA_ARGS__)
#else
#define D_SerialBegin(bauds)
#define D_print(...)
#define D_println(...)
#define D_printf(...)
#endif

#define SOUND_SENSOR_PIN 2
#define SOUND_SAMPLES 1024

class Led {
public:
  int pin;

  explicit Led(const int pin): pin(pin) {
  }

  void setup() const {
    pinMode(pin, OUTPUT);
  }

  void turnOn() const {
    digitalWrite(pin, HIGH);
  }

  void turnOff() const {
    digitalWrite(pin, LOW);
  }
};

Led dums[] = {Led(42), Led(39), Led(38), Led(37), Led(47)};
Led teks[] = {Led(41), Led(40), Led(21)};
Led leds[] = {dums[0], dums[1], dums[2], dums[3], dums[4], teks[0], teks[1], teks[2]};

void bayo() {
  constexpr int beat = 220;
  constexpr int onOffDelay = 100;

  // dum+
  for (auto led: leds) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: leds) {
    led.turnOff();
  }
  delay(2 * beat - onOffDelay);

  // tek
  for (auto led: teks) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: teks) {
    led.turnOff();
  }
  delay(1 * beat - onOffDelay);

  // ke
  for (auto led: teks) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: teks) {
    led.turnOff();
  }
  delay(1 * beat - onOffDelay);

  // dum+
  for (auto led: leds) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: leds) {
    led.turnOff();
  }
  delay(2 * beat - onOffDelay);

  // ke+
  for (auto led: teks) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: teks) {
    led.turnOff();
  }
  delay(2 * beat - onOffDelay);
}

void setup() {
  D_SerialBegin(115200);

  pinMode(SOUND_SENSOR_PIN, INPUT);

  for (auto led: leds) {
    led.setup();
  }

  D_println("Setup done");

  bayo();
}

void loop() {
  int soundTotal = 0;
  for (auto i = 0; i < SOUND_SAMPLES; i++) {
    soundTotal += analogRead(SOUND_SENSOR_PIN);
  }
  const int soundAverage = soundTotal / SOUND_SAMPLES;

  if (PLOT) {
    D_println(soundAverage);
    return;
  }

  if (soundAverage == 0) {
    // treat as not playing below this threshold
    for (auto led: leds) {
      led.turnOff();
    }

    return;
  }

  D_printf("Sound average: %d\n", soundAverage);

  D_println("tek");
  for (auto led: teks) {
    led.turnOn();
  }

  if (soundAverage < 2000) {
    return;
  }

  // treat as dum above this threshold
  D_println("dum");
  for (auto led: dums) {
    led.turnOn();
  }
}
