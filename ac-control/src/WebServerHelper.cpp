#include "WebServerHelper.h"

WebServerHelper::WebServerHelper(
  WebServerTemplate &webServerTemplate
): webServer(80),
   webServerTemplate(webServerTemplate) {
}

void WebServerHelper::setup() {
  webServer.begin();

  webServer.on("/", HTTP_GET, [this] {
    webServer.send(200, "text/html", webServerTemplate.index());
  });

  webServer.onNotFound([this] {
    // standard not found in browser.
    webServer.send(404, "text/html", webServerTemplate.notFound());
  });
}

void WebServerHelper::loop() {
  webServer.handleClient();
}
