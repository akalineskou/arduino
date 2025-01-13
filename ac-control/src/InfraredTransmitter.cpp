#include "Directive.h"
#include "InfraredTransmitter.h"

InfraredTransmitter::InfraredTransmitter(const int pin, const ACMode &acMode): pin(pin), acMode(acMode), irSend(pin) {
  lastACCommand = Off;

  lightToggled = false;
}

void InfraredTransmitter::setup() {
  irSend.begin();
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand, const bool forceCommand) {
  if (acCommand == lastACCommand && !forceCommand) {
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
    // as a backup, before sending off, set temperature to stop values
    if (acMode == Cold) {
      // Temp: 30C
      irSend.setTemp(30);
    } else {
      // Temp: 16C
      irSend.setTemp(16);
    }

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

#if DEBUG
  Serial.printf("Sending A/C command: %s.\n", ACCommands[acCommand]);
  Serial.println(irSend.toString().c_str());
#endif

#if !DEBUG || DEBUG_IR_SEND
  irSend.send();
#endif
}
