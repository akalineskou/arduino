#include "Arduino.h"

#define DEBUG 0

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__);
#define D_println(...)  Serial.println(__VA_ARGS__)
#define D_printf(...)  Serial.printf(__VA_ARGS__)
#else
#define D_SerialBegin(bauds)
#define D_println(...)
#define D_printf(...)
#endif

#define SENSOR_PIN 1

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

Led dum1(42);
Led dum2(39);
Led dum3(38);
Led dum4(37);
Led dum5(44);

Led dums[] = {dum1, dum2, dum3, dum4, dum5};

Led tek1(41);
Led tek2(40);
Led tek3(21);

Led teks[] = {tek1, tek2, tek3};

Led leds[] = {dum1, dum2, dum3, dum4, dum5, tek1, tek2, tek3};

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
  // fast baud for IR receiver
  D_SerialBegin(115200);

  pinMode(SENSOR_PIN, INPUT);

  for (auto led: leds) {
    led.setup();
  }

  D_println("Setup done");

  bayo();
}

void loop() {
  const unsigned long startMillis = millis();

  // get sample average in 25ms
  int sample = 0, i = 0;
  while (millis() - startMillis < 25) {
    sample += analogRead(SENSOR_PIN);
    i++;
  }
  const int samplesAverage = sample / i;

  if (samplesAverage < 25) {
    // treat as not playing below this threshold
    for (auto led: leds) {
      led.turnOff();
    }

    return;
  }

  D_printf("Samples average: %d\n", samplesAverage);

  // treat as tek
  D_println("tek");

  for (auto led: teks) {
    led.turnOn();
  }

  if (samplesAverage < 1500) {
    return;
  }

  // treat as dum above this threshold
  D_println("dum");

  for (auto led: dums) {
    led.turnOn();
  }
}
