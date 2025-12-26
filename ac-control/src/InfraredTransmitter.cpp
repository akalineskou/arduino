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

void InfraredTransmitter::sendCommand(ACCommand acCommand) {
  lastACCommand = acCommand;

#if APP_ENV == APP_ENV_LIVIN_ROOM
  irSend.setFan(kHaierAcYrw02FanHigh);

  if (acMode == Cold) {
    irSend.setMode(kHaierAcYrw02Cool);
    irSend.setSwingV(kHaierAc160SwingVTop);
  } else {
    irSend.setMode(kHaierAcYrw02Heat);
    irSend.setSwingV(kHaierAc160SwingVLow);
  }
#elif APP_ENV == APP_ENV_BEDROOM
  irSend.setFan(kHaierAcYrw02FanHigh);

  if (acCommand == Stop) {
    // set fan to low since fan does not turn off
    irSend.setFan(kHaierAcYrw02FanLow);
  }

  if (acMode == Cold) {
    irSend.setMode(kHaierAcYrw02Cool);
    irSend.setSwingV(kHaierAcYrw02SwingVTop);
  } else {
    irSend.setMode(kHaierAcYrw02Heat);
    irSend.setSwingV(kHaierAcYrw02SwingVBottom);
  }
#elif APP_ENV == APP_ENV_OTHER
  irSend.setFan(kBosch144Fan100);

  if (acCommand == Stop) {
    irSend.setFan(kBosch144Fan20);
  }

  if (acMode == Cold) {
    irSend.setMode(kBosch144Cool);
  } else {
    irSend.setMode(kBosch144Heat);
  }
#endif

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
#if APP_ENV == APP_ENV_LIVIN_ROOM || APP_ENV == APP_ENV_BEDROOM
    irSend.off();
#elif APP_ENV == APP_ENV_OTHER
    irSend.setPower(false);
#endif
  } else {
    // Power: On
#if APP_ENV == APP_ENV_LIVIN_ROOM || APP_ENV == APP_ENV_BEDROOM
    irSend.on();
#elif APP_ENV == APP_ENV_OTHER
    irSend.setPower(true);
#endif
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
#if APP_ENV == APP_ENV_LIVIN_ROOM
      // only needs to happen the first time it is turned on
      irSend.setLightToggle(true);
#endif

      lightToggled = true;
    }
  }

#if !APP_DEBUG || APP_DEBUG_IR_SEND
  irSend.send();
#endif

  databaseHelper.insertLog(__FILENAME__, __LINE__, "Sending A/C command: %s.", ACCommands[acCommand]);
#if APP_DEBUG
  Serial.printf("Sending A/C command: %s.\n", ACCommands[acCommand]);
  Serial.println(irSend.toString().c_str());
#endif

  databaseHelper.updatePreferenceByType(IrLastACCommand, ACCommands[acCommand]);
  databaseHelper.updatePreferenceByType(IrLightToggled, reinterpret_cast<void*>(lightToggled));
}
