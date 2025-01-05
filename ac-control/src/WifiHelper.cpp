#include <WiFi.h>
#include "Secrets.h"
#include "Serial.h"
#include "WifiHelper.h"

void WifiHelper::setup() {
  D_printf("Connecting to %s.\n", wifiSSID);

  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(500);
  }

  D_println("Connected.");
  D_printf("IP address: %s.\n", WiFi.localIP().toString().c_str());
}
