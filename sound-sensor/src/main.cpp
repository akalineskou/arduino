#include <Arduino.h>

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
#if DEBUG || PLOT
  Serial.begin(115200);
#endif

  pinMode(SOUND_SENSOR_PIN, INPUT);

  for (auto led: leds) {
    led.setup();
  }

#if DEBUG && !PLOT
  Serial.println("Setup done");
#endif

  bayo();
}

void loop() {
  int soundTotal = 0;
  for (auto i = 0; i < SOUND_SAMPLES; i++) {
    soundTotal += analogRead(SOUND_SENSOR_PIN);
  }
  const int soundAverage = soundTotal / SOUND_SAMPLES;

#if PLOT
  Serial.printf("Dum:%d,Average:%d\n", DUM_THRESHOLD, soundAverage);
#endif

  if (soundAverage < SOUND_MIN_THRESHOLD) {
    // treat as not playing below this threshold
    for (auto led: leds) {
      led.turnOff();
    }

    return;
  }

#if DEBUG && !PLOT
  Serial.printf("Sound average: %d\n", soundAverage);
#endif

#if DEBUG && !PLOT
  Serial.println("tek");
#endif

  for (auto led: tek) {
    led.turnOn();
  }

  if (soundAverage <= DUM_THRESHOLD) {
    return;
  }

#if DEBUG && !PLOT
  Serial.println("dum");
#endif

  for (auto led: dum) {
    led.turnOn();
  }
}
