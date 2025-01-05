#include <Arduino.h>

#define DEBUG 0
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
#define SOUND_SAMPLES 768
#define SOUND_MIN_THRESHOLD 50
#define DUM_THRESHOLD 2400

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

Led dum[] = {Led(42), Led(39), Led(38), Led(37), Led(47)};
Led tek[] = {Led(41), Led(40), Led(21)};
Led leds[] = {dum[0], dum[1], dum[2], dum[3], dum[4], tek[0], tek[1], tek[2]};

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
  for (auto led: tek) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: tek) {
    led.turnOff();
  }
  delay(1 * beat - onOffDelay);

  // ke
  for (auto led: tek) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: tek) {
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
  for (auto led: tek) {
    led.turnOn();
  }
  delay(onOffDelay);
  for (auto led: tek) {
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

  if (!PLOT) D_println("Setup done");

  bayo();
}

void loop() {
  int soundTotal = 0;
  for (auto i = 0; i < SOUND_SAMPLES; i++) {
    soundTotal += analogRead(SOUND_SENSOR_PIN);
  }
  const int soundAverage = soundTotal / SOUND_SAMPLES;

  if (PLOT) D_printf("Dum:%d,Average:%d\n", DUM_THRESHOLD, soundAverage);

  if (soundAverage < SOUND_MIN_THRESHOLD) {
    // treat as not playing below this threshold
    for (auto led: leds) {
      led.turnOff();
    }

    return;
  }

  if (!PLOT) D_printf("Sound average: %d\n", soundAverage);

  if (!PLOT) D_println("tek");

  for (auto led: tek) {
    led.turnOn();
  }

  if (soundAverage <= DUM_THRESHOLD) {
    return;
  }

  if (!PLOT) D_println("dum");

  for (auto led: dum) {
    led.turnOn();
  }
}
