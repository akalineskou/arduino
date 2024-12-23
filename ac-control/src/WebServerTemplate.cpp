#include "WebServerTemplate.h"

WebServerTemplate::WebServerTemplate(
  TemperatureSensor &temperatureSensor
): temperatureSensor(temperatureSensor) {
}

String WebServerTemplate::index() {
  auto html = String(R"==(
<html>
<head>
  <title>A/C Control</title>
  <meta charset="utf-8">
</head>
<body>
  <p>Hello.</p>
  <p>Temperature __TEMPERATURE__.</p>
</body>
)==");

  html.replace("__TEMPERATURE__", TemperatureSensor::formatTemperature(temperatureSensor.temperature));

  return html;
}

String WebServerTemplate::notFound() {
  return String(R"==(
<html>
<head>
  <title>Resource not found</title>
</head>
<body>
  <p>The resource was not found.</p>
  <p><a href="/">Start again</a></p>
</body>
)==");
}
