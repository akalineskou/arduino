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

  // Fan: 1 (High)
  irSend.setFan(IR_SEND_FAN);

  if (acMode == Cold) {
    // Mode: 2 (Cool)
    irSend.setMode(IR_SEND_MODE_COOL);
    // Swing(V): 1 (Top)
    irSend.setSwingV(IR_SEND_MODE_COOL_SWING_V);
  } else {
    // Mode: 4 (Heat)
    irSend.setMode(IR_SEND_MODE_HEAT);
    // Swing(V): 8 (Low)
    irSend.setSwingV(IR_SEND_MODE_HEAT_SWING_V);
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
#if IR_SEND_HAS_TOGGLE_LIGHT
      // only needs to happen the first time it is turned on
      irSend.setLightToggle(true);
#endif

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
