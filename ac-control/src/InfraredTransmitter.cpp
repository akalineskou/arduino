#include "DatabaseHelper.h"
#include "Directive.h"
#include "InfraredTransmitter.h"
#include "Preference.h"

InfraredTransmitter::InfraredTransmitter(const int pin, DatabaseHelper &databaseHelper, ACMode &acMode):
    pin(pin),
    databaseHelper(databaseHelper),
    acMode(acMode),
    irSend(pin) {
  lastACCommand = Off;

  lightToggled = false;
}

void InfraredTransmitter::setup() {
  irSend.begin();
}

void InfraredTransmitter::sendCommand(const ACCommand acCommand) {
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
    // reset on turn off
    lightToggled = false;
  } else {
    if (!lightToggled) {
      // only needs to happen the first time it is turned on
      irSend.setLightToggle(true);

      lightToggled = true;
    }
  }

#if !APP_DEBUG || APP_DEBUG_IR_SEND
  irSend.send();
#endif

#if APP_DEBUG
  Serial.printf("Sending A/C command: %s.\n", ACCommands[acCommand]);
  Serial.println(irSend.toString().c_str());
#endif

  databaseHelper.updatePreferenceByType(IrLastACCommand, ACCommands[acCommand]);
  databaseHelper.updatePreferenceByType(IrLightToggled, reinterpret_cast<void*>(lightToggled));
}
