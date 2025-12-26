#include <WiFi.h>

#include "Directive.h"
#include "Secrets.h"
#include "WifiHelper.h"

WifiHelper::WifiHelper(DatabaseHelper &databaseHelper): databaseHelper(databaseHelper) {}

void WifiHelper::setup() {
  WiFiClass::mode(WIFI_STA);
  WiFi.begin(wifiSSID, wifiPassword);

  auto attempts = 0;

  while (WiFiClass::status() != WL_CONNECTED && ++attempts < 10) {
    delay(100);

    yield();
  }

  if (WiFiClass::status() != WL_CONNECTED) {
    databaseHelper.insertLog(__FILENAME__, __LINE__, "Could not connected to %s in time.", wifiSSID);
#if APP_DEBUG
    Serial.printf("Could not connected to %s in time.\n", wifiSSID);
#endif
    return;
  }

  databaseHelper
    .insertLog(__FILENAME__, __LINE__, "Connected to %s (%s).", wifiSSID, WiFi.localIP().toString().c_str());
#if APP_DEBUG
  Serial.printf("Connected to %s (%s).\n", wifiSSID, WiFi.localIP().toString().c_str());
#endif
}
