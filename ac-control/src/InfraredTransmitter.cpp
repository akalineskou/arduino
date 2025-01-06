#include "InfraredTransmitter.h"
#include "Serial.h"

InfraredTransmitter::InfraredTransmitter(
  const int pin,
  const ACMode &acMode
): pin(pin),
   acMode(acMode),
   irSend(pin),
   timeDelay(millisDelay()) {
  lastACCommand = Off;

  lightToggled = false;
}

void InfraredTransmitter::setup() {
  irSend.begin();

  // allow IR transmit every 0.5s
  timeDelay.start(0.5 * 1000);
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceSend) {
  if (!timeDelay.justFinished() && !forceSend) {
    return;
  }
  timeDelay.repeat();

  if (acCommand == lastACCommand) {
    // same A/C command, ignoring send
    return;
  }
  lastACCommand = acCommand;

  /// /.pio -> ir_Haier.cpp -> IRHaierAC160::IRHaierAC160
  // Fan: 1 (High)
  irSend.setFan(kHaierAcYrw02FanHigh);

  if (acMode == Cold) {
    // Mode: 2 (Cool)
    irSend.setMode(kHaierAcYrw02Cool);
    // Swing(V): 1 (Top)
    irSend.setSwingV(kHaierAc160SwingVTop);
  } else {
    // Mode: 4 (Heat)
    irSend.setMode(kHaierAcYrw02Heat);
    // Swing(V): 8 (Low)
    irSend.setSwingV(kHaierAc160SwingVLow);
  }

  if (acCommand == Off) {
    // Power: Off
    irSend.off();
  } else {
    // Power: On
    irSend.on();
  }

  if ((acCommand == Start && acMode == Cold) || (acCommand == Stop && acMode == Heat)) {
    // Temp: 16C
    irSend.setTemp(16);
  } else if ((acCommand == Start && acMode == Heat) || (acCommand == Stop && acMode == Cold)) {
    // Temp: 30C
    irSend.setTemp(30);
  }

  if (acCommand == Off) {
    lightToggled = false;
  } else {
    if (!lightToggled) {
      irSend.setLightToggle(true);

      lightToggled = true;
    }
  }

  D_printf("Sending A/C command: %s.\n", ACCommands[acCommand]);
  D_println(irSend.toString().c_str());

  irSend.send();
}
