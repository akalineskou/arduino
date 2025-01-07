#include <WiFi.h>
#include "WifiHelper.h"

void WifiHelper::setup(const char *wifiSSID, const char *wifiPassword) {
#if DEBUG
  Serial.printf("Connecting to %s.\n", wifiSSID);
#endif

  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(500);
  }

#if DEBUG
  Serial.println("Connected.");
  Serial.printf("IP address: %s.\n", WiFi.localIP().toString().c_str());
#endif
}
