#include <IRac.h>

#include "Directive.h"
#include "InfraredReceiver.h"

InfraredReceiver::InfraredReceiver(const int pin): pin(pin) {}

void InfraredReceiver::setup() {}

void InfraredReceiver::loop() {
#if APP_DEBUG
#endif
}
