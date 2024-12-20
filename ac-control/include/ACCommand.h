#pragma once

enum ACCommand {
  Null,
  Off,
  Start,
  Stop,
};

static const char *ACCommands[] = {
  "Null", "Off", "Start", "Stop",
};
