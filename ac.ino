#include <IRsend.h>
#include <DHT.h>

const int tempPin = 0;
const int irPin = 16;
const int buttonPin = 4;

DHT dht1(tempPin, DHT22);
IRsend irsend(irPin);

String acState = "";
unsigned long acTime = 0;

int tempState = 0;
const int tempTarget = 20.5 * 10;
const int tempThresholdUp = 0.5 * 10;
const int tempThresholdDown = 1 * 10;
unsigned long tempTime = 0;

bool buttonEnabled = false;
unsigned long buttonTime = 0;

void setup() {
  delay(150);

  Serial.begin(115200);

  while (!Serial) {
    delay(100);
  }

  dht1.begin();
  irsend.begin();
  irReceiverSetup();
  pinMode(buttonPin, INPUT);
}

void loop() {
  buttonCheck();

  irTransmitterSendCommandByTemp();

  if (Serial.available()) {
    irTransmitterSendCommand(Serial.readStringUntil('\n'));
  }

  irReceiverRead();
}

void buttonCheck() {
  if (digitalRead(buttonPin) != HIGH) {
    return;
  }

  if (millis() - buttonTime < 2 * 1000) {
    // get button readings every 2 seconds
    return;
  }
  buttonTime = millis();

  buttonEnabled = !buttonEnabled;

  acTime = 99999;

  if (!buttonEnabled) {
    irTransmitterSendCommand("off");
  } else {
    irTransmitterSendCommand("heatStart");
  }
}

int tempGet(DHT dht, int sensor) {
  if (millis() - tempTime < 2 * 1000) {
    // get temp readings every 2 seconds
    return -1;
  }
  tempTime = millis();

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

void irTransmitterSendCommandByTemp() {
  if (!buttonEnabled) {
    return;
  }

  int temp = tempGet(dht1, 1);
  if (temp < 0) {
    return;
  }

  if (acState == "heatStart") {
    if (temp >= (tempTarget + tempThresholdUp)) {
      irTransmitterSendCommand("heatStop");
    }
  } else if (acState == "heatStop") {
    if (temp <= (tempTarget - tempThresholdDown)) {
      irTransmitterSendCommand("heatStart");
    }
  } else {
    irTransmitterSendCommand("heatStart");
  }
}

struct ACData {
  // off
  uint8_t off[kHaierAC160StateLength];

  // heat 30°C
  uint8_t heatStart[kHaierAC160StateLength];
  // heat 16°C
  uint8_t heatStop[kHaierAC160StateLength];
};

ACData acData = {
  // off
  { 0xA6, 0xE8, 0xB7, 0x00, 0x00, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x05, 0xEA, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 },

  // heatStart -> 30°C
  { 0xA6, 0xE8, 0xB6, 0x00, 0x7B, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x5F, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 },
  // heatStop -> 16°C
  // @TODO: check ac led light is disabled in remote control
  { 0xA6, 0x08, 0xB7, 0x00, 0x45, 0x20, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00, 0x01, 0x4B, 0xB5, 0x00, 0x40, 0x00, 0x00, 0xF5 },
};

void irTransmitterSendCommand(String command) {
  if (millis() - acTime < 500) {
    // ir transmit every 0.5s
    return;
  }
  acTime = millis();

  if (command == acState) {
    // Same AC command, ignoring send
    return;
  }
  acState = command;

  if (command == "off") {
    irsend.sendHaierAC160(acData.off);
  } else if (command == "heatStart") {
    irsend.sendHaierAC160(acData.heatStart);
  } else if (command == "heatStop") {
    irsend.sendHaierAC160(acData.heatStop);
  } else {
    Serial.printf("Invalid command %s\n", command);

    return;
  }

  Serial.printf("Sending AC: %s\n", command);
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
