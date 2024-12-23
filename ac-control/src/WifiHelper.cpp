#include <WiFi.h>
#include "WifiHelper.h"
#include "Secrets.h"

void WifiHelper::setup() {
  Serial.printf("Connecting to %s.\n", wifiSSID);

  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(500);
  }

  Serial.println("Connected.");
  Serial.printf("IP address: %s.\n", WiFi.localIP().toString().c_str());
}
