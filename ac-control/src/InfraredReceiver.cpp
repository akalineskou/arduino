#include <IRac.h>

#include "Directive.h"
#include "InfraredReceiver.h"

InfraredReceiver::InfraredReceiver(const int pin, ACControl &acControl, TemperatureData &temperatureData):
    pin(pin),
    acControl(acControl),
    temperatureData(temperatureData),
    irRecv(pin) {}

void InfraredReceiver::setup() {
  irRecv.enableIRIn();
}

void InfraredReceiver::loop() {
  decode_results results{};

  if (!irRecv.decode(&results)) {
    return;
  }

#if APP_DEBUG && APP_DEBUG_IR_RECEIVE
  Serial.println(uint64ToString(results.value, HEX));
#endif

  if (results.value == 0xFF02FD) {
    // OK -> enable/disable
    if (acControl.enabled) {
      acControl.disable();
    } else {
      acControl.enable();
    }
  } else if (results.value == 0xFF629D) {
    // TOP -> increase target
    temperatureData.temperatureTargetIncrease();
  } else if (results.value == 0xFF22DD) {
    // LEFT -> stop
    acControl.stop();
  } else if (results.value == 0xFFA857) {
    // DOWN -> decrease target
    temperatureData.temperatureTargetDecrease();
  } else if (results.value == 0xFFC23D) {
    // RIGHT -> start
    acControl.start();
  } else if (results.value == 0xFF42BD) {
    // *
  } else if (results.value == 0xFF4AB5) {
    // 0
  } else if (results.value == 0xFF52AD) {
    // # -> change mode
    acControl.changeMode();
  }

  irRecv.resume();
}
