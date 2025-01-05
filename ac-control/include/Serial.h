#pragma once

#define DEBUG 0

#if DEBUG
#define D_SerialBegin(...) Serial.begin(__VA_ARGS__);
#define D_print(...)  D_print(__VA_ARGS__)
#define D_println(...)  D_println(__VA_ARGS__)
#define D_printf(...)  D_printf(__VA_ARGS__)
#else
#define D_SerialBegin(bauds)
#define D_print(...)
#define D_println(...)
#define D_printf(...)
#endif
