#include <WiFi.h>
#include "Directive.h"
#include "WifiHelper.h"

void WifiHelper::setup(const char *wifiSSID, const char *wifiPassword) {
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(100);
  }

#if DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
#endif
}
