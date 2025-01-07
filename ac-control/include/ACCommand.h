#pragma once

enum ACCommand {
  Off,
  Start,
  Stop,
};

static const char *ACCommands[] = {
  "Off", "Start", "Stop",
};

inline ACCommand sToACCommand(const char *acCommand) {
  if (strcasecmp(acCommand, "Off") == 0) {
    return Off;
  }
  if (strcasecmp(acCommand, "Start") == 0) {
    return Start;
  }
  return Stop;
}
