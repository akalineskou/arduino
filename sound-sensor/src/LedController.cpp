#include <Arduino.h>

#include "Directive.h"
#include "LedController.h"

LedController::LedController(const int dumPin, const int tekPin): dum(Led(dumPin)), tek(Led(tekPin)) {}

void LedController::setup() const {
  dum.setup();
  tek.setup();
}

void LedController::on(const Beat beat) const {
#if APP_DEBUG
  Serial.println(Beats[beat]);
#endif

  switch (beat) {
    case Dum:
      dum.on();
      break;

    case Tek:
      tek.on();
      break;
  }
}

void LedController::off(const Beat beat) const {
  switch (beat) {
    case Dum:
      dum.off();
      break;

    case Tek:
      tek.off();
      break;
  }
}

int LedController::offDelay(const Beat beat) {
  return beat == Dum ? 150 : 50;
}
