#include <WiFi.h>

#include "Directive.h"
#include "WifiHelper.h"

void WifiHelper::setup(const char* wifiSSID, const char* wifiPassword) {
  WiFiClass::mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  while (WiFiClass::status() != WL_CONNECTED) {
    delay(100);
  }

#if APP_DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
#endif

  yield();
}
