#pragma once

enum Preference {
  AcMode,
  AcEnabled,
  AcTemperatureStart,
  AcTemperatureStop,
  AcTurnOffInsteadOfStop,
  IrLastACCommand,
  IrLightToggled,
  TdTemperatureTarget,
};

static const char* xPreferences[] = {
  "acMode",
  "acEnabled",
  "acTemperatureStart",
  "acTemperatureStop",
  "acTurnOffInsteadOfStop",
  "irLastACCommand",
  "irLightToggled",
  "tdTemperatureTarget",
};
