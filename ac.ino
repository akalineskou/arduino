#include <IRsend.h>
#include <DHT.h>

DHT dht1(4, DHT22);
IRsend irsend(16);

String acState = "";
int tempState = 0;
const int tempThreshold = 200;

void setup() {
  delay(150);

  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  dht1.begin();
  irsend.begin();

  irReceiverSetup();

  // start after setting up
  irTransmitterSendCommandByTemp(0);
}

void loop() {
  // get temp readings every 2 seconds
  delay(2 * 1000);

  irTransmitterSendCommandByTemp(tempGet(dht1, 1));

  if (Serial.available()) {
    irTransmitterSendCommand(Serial.readStringUntil('\n'));
  }

  irReceiverRead();
}

int tempGet(DHT dht, int sensor) {
  float tempFloat = dht.readTemperature();

  if (isnan(tempFloat)) {
    Serial.printf("Failed to read from DHT22 sensor %d!\n", sensor);

    return -1;
  }

  int tempInt = (int)round(tempFloat * 10);

  if (tempInt != tempState) {
    Serial.printf("Sensor %d: %.2f°C, %d\n", sensor, tempFloat, tempInt);
  }

  tempState = tempInt;

  return tempInt;
}

void irTransmitterSendCommandByTemp(int temp) {
  if (temp < 0) {
    return;
  }

  irTransmitterSendCommand(temp < tempThreshold ? "start" : "stop");
}

void irTransmitterSendCommand(String command) {
  if (command == acState) {
    // Same AC command, ignoring send
    return;
  }

  acState = command;

  Serial.printf("AC state: %s\n", acState);
  Serial.printf("AC command: %s\n", command);

  if (command == "start") {
    // heat 30°C
    const uint8_t data[kHaierAC160StateLength] = { 0xA6, 0xE8, 0xB6, 0x00, 0x7B, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 };
    irsend.sendHaierAC160(data);
  } else if (command == "stop") {
    // heat 16°C
    const uint8_t data[kHaierAC160StateLength] = { 0xA6, 0x08, 0xB7, 0x00, 0x45, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4B, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 };
    irsend.sendHaierAC160(data);
  } else if (command == "off") {
    // off
    const uint8_t data[kHaierAC160StateLength] = { 0xA6, 0xE8, 0xB7, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x05, 0xEA, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 };
    irsend.sendHaierAC160(data);
  } else {
    Serial.printf("Invalid command %s\n", command);

    return;
  }
}

// #include <IRrecv.h>
// #include <IRutils.h>
// #include <IRac.h>
// #include <IRtext.h>

// IRrecv irrecv(2, 1024, 50, true);
// decode_results results;

void irReceiverSetup() {
  // #if DECODE_HASH
  //   irrecv.setUnknownThreshold(12);
  // #endif

  //   irrecv.setTolerance(kTolerance);  // Override the default tolerance.
  //   irrecv.enableIRIn();
}

void irReceiverRead() {
  // // Check if the IR code has been received.
  // if (!irrecv.decode(&results)) {
  //   return;
  // }

  // // Check if we got an IR message that was to big for our capture buffer.
  // // Display the basic output of what we found.
  // Serial.print(resultToHumanReadableBasic(&results));

  // // Display any extra A/C info if we have it.
  // String description = IRAcUtils::resultAcToString(&results);
  // if (description.length()) {
  //   Serial.println(D_STR_MESGDESC ": " + description);
  // }
  // yield();  // Feed the WDT as the text output can take a while to print.

  // // Output the results as source code
  // Serial.println(resultToSourceCode(&results));
  // Serial.println();  // Blank line between entries
  // yield();           // Feed the WDT (again)
}
