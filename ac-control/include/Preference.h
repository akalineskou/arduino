#pragma once

enum Preference {
  AcMode,
  AcEnabled,
  AcTemperatureStart,
  AcTemperatureStop,
  IrLastACCommand,
  IrLightToggled,
  TdTemperatureTarget,
};

static const char* xPreferences[] = {
  "acMode",
  "acEnabled",
  "acTemperatureStart",
  "acTemperatureStop",
  "irLastACCommand",
  "irLightToggled",
  "tdTemperatureTarget",
};
