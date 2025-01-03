#include "Arduino.h"

#define SENSOR_PIN 1
#define LED_DUM_PIN1 41
#define LED_DUM_PIN2 40
#define LED_TEK_PIN1 42
#define LED_TEK_PIN2 39
#define LED_TEK_PIN3 38

void setup() {
  // fast baud for IR receiver
  Serial.begin(115200);

  // wait for serial monitor to start completely.
  while (!Serial) {
    delay(100);
  }

  pinMode(SENSOR_PIN, INPUT);

  pinMode(LED_DUM_PIN1,OUTPUT);
  pinMode(LED_DUM_PIN2,OUTPUT);
  pinMode(LED_TEK_PIN1,OUTPUT);
  pinMode(LED_TEK_PIN2,OUTPUT);
  pinMode(LED_TEK_PIN3,OUTPUT);

  Serial.println("setup done");

  // dum+
  digitalWrite(LED_DUM_PIN1,HIGH);
  digitalWrite(LED_DUM_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);
  delay(50);
  digitalWrite(LED_DUM_PIN1,LOW);
  digitalWrite(LED_DUM_PIN2,LOW);
  digitalWrite(LED_TEK_PIN1,LOW);
  digitalWrite(LED_TEK_PIN2,LOW);
  digitalWrite(LED_TEK_PIN3,LOW);
  delay(2 * 100);

  // tek
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);
  delay(50);
  digitalWrite(LED_TEK_PIN1,LOW);
  digitalWrite(LED_TEK_PIN2,LOW);
  digitalWrite(LED_TEK_PIN3,LOW);
  delay(1 * 100);

  // ke
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);
  delay(50);
  digitalWrite(LED_TEK_PIN1,LOW);
  digitalWrite(LED_TEK_PIN2,LOW);
  digitalWrite(LED_TEK_PIN3,LOW);
  delay(1 * 100);

  // dum+
  digitalWrite(LED_DUM_PIN1,HIGH);
  digitalWrite(LED_DUM_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);
  delay(50);
  digitalWrite(LED_DUM_PIN1,LOW);
  digitalWrite(LED_DUM_PIN2,LOW);
  digitalWrite(LED_TEK_PIN1,LOW);
  digitalWrite(LED_TEK_PIN2,LOW);
  digitalWrite(LED_TEK_PIN3,LOW);
  delay(2 * 100);

  // ke+
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);
  delay(50);
  digitalWrite(LED_TEK_PIN1,LOW);
  digitalWrite(LED_TEK_PIN2,LOW);
  digitalWrite(LED_TEK_PIN3,LOW);
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
    digitalWrite(LED_DUM_PIN1,LOW);
    digitalWrite(LED_DUM_PIN2,LOW);
    digitalWrite(LED_TEK_PIN1,LOW);
    digitalWrite(LED_TEK_PIN2,LOW);
    digitalWrite(LED_TEK_PIN3,LOW);

    return;
  }

  // Serial.printf("Samples average: %d\n", samplesAverage);

  // treat as tek
  Serial.println("tek");
  digitalWrite(LED_TEK_PIN1,HIGH);
  digitalWrite(LED_TEK_PIN2,HIGH);
  digitalWrite(LED_TEK_PIN3,HIGH);

  if (samplesAverage < 1500) {
    return;
  }

  // treat as dum above this threshold
  Serial.println("dum");
  digitalWrite(LED_DUM_PIN1,HIGH);
  digitalWrite(LED_DUM_PIN2,HIGH);
}
