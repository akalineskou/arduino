#pragma once

#include "Directive.h"

#if APP_ENV == APP_ENV_LIVIN_ROOM
auto wifiSSID = "";
auto wifiPassword = "";
#elif APP_ENV == APP_ENV_BEDROOM
auto wifiSSID = "";
auto wifiPassword = "";
#elif APP_ENV == APP_ENV_OTHER
auto wifiSSID = "";
auto wifiPassword = "";
#endif
