#include <WiFi.h>

#include "Directive.h"
#include "Secrets.h"
#include "WifiHelper.h"

void WifiHelper::setup() {
  WiFiClass::mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  auto attempts = 0;

  while (WiFiClass::status() != WL_CONNECTED && ++attempts < 10) {
    delay(100);

    yield();
  }

  if (WiFiClass::status() != WL_CONNECTED) {
#if APP_DEBUG
    Serial.printf("Could not connected to %s in time.\n", wifiSSID);
#endif
    return;
  }

#if APP_DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
#endif
}
