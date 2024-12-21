#include <IRutils.h>
#include <IRac.h>
#include "InfraredReceiver.h"

void InfraredReceiver::setup() {
  irRecv.setUnknownThreshold(12);
  irRecv.setTolerance(kTolerance);
  irRecv.enableIRIn();
}

void InfraredReceiver::loop() {
  decode_results results{};

  // Check if the IR code has been received.
  if (!irRecv.decode(&results)) {
    return;
  }

  // Check if we got an IR message that was too big for our capture buffer.
  // Display the basic output of what we found.
  Serial.print(resultToHumanReadableBasic(&results));

  // Display any extra A/C info if we have it.
  const String description = IRAcUtils::resultAcToString(&results);
  if (description.length()) {
    Serial.printf("Message description: %s.\n", description.c_str());
  }
  yield(); // Feed the WDT as the text output can take a while to print.

  // Output the results as source code
  Serial.println(resultToSourceCode(&results));
  Serial.println();
  yield(); // Feed the WDT (again)
}
