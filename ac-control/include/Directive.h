#pragma once

#define APP_DEBUG false
#define APP_DEBUG_HEAP false
#define APP_DEBUG_TEMPERATURE_CHANGE false
#define APP_DEBUG_RANDOM_TEMPERATURE_SENSOR false
#define APP_DEBUG_IR_SEND false
#define APP_DEBUG_DATABASE_TEMPERATURE_CHECK false

#define APP_REBOOT_DELAY (12 * 60 * 60 * 1000)
#define APP_DEBUG_HEAP_DELAY (10 * 1000)
#define APP_AC_CONTROL_TURN_OFF_DELAY (3 * 60 * 60 * 1000)
#define APP_AC_CONTROL_CHECK_DELAY (5 * 60 * 1000)
#define APP_TEMPERATURE_SENSOR_TEMPERATURE_DELAY (10 * 1000)
#define APP_TEMPERATURE_SENSOR_HUMIDITY_DELAY (60 * 1000)
#define APP_TEMPERATURE_SENSOR_DATABASE_DELAY (10 * 1000)

#define APP_ENV_LIVIN_ROOM 1
#define APP_ENV_BEDROOM 2
#define APP_ENV_OTHER 3

#if APP_ENV == APP_ENV_LIVIN_ROOM
  #define APP_IR_TRANSMITTER true

  #define PIN_TEMP_SENSOR 32
  #define PIN_IR_TRANSMITTER 22
  #define PIN_IR_RECEIVER 2
  #define PIN_SD_SCK 18
  #define PIN_SD_MISO 19
  #define PIN_SD_MOSI 23
  #define PIN_SD_SS 5

  #define DHT_TYPE DHT22

/// /.pio -> ir_Haier.cpp -> IRHaierAC160::IRHaierAC160
  #define IR_SEND_CLASS IRHaierAC160
  #define IR_SEND_FAN kHaierAcYrw02FanHigh
  #define IR_SEND_MODE_COOL kHaierAcYrw02Cool
  #define IR_SEND_MODE_COOL_SWING_V kHaierAc160SwingVTop
  #define IR_SEND_MODE_HEAT kHaierAcYrw02Heat
  #define IR_SEND_MODE_HEAT_SWING_V kHaierAc160SwingVLow
  #define IR_SEND_HAS_TOGGLE_LIGHT true
#elif APP_ENV == APP_ENV_BEDROOM
  #define APP_IR_TRANSMITTER false

  #define PIN_TEMP_SENSOR 10
  #define PIN_IR_TRANSMITTER 9
  #define PIN_IR_RECEIVER 5
  #define PIN_SD_SCK 36
  #define PIN_SD_MISO 37
  #define PIN_SD_MOSI 35
  #define PIN_SD_SS 38

  #define DHT_TYPE DHT22

/// /.pio -> ir_Haier.cpp -> IRHaierACYRW02::IRHaierACYRW02
  #define IR_SEND_CLASS IRHaierACYRW02
  #define IR_SEND_FAN kHaierAcYrw02FanHigh
  #define IR_SEND_MODE_COOL kHaierAcYrw02Cool
  #define IR_SEND_MODE_COOL_SWING_V kHaierAcYrw02SwingVTop
  #define IR_SEND_MODE_HEAT kHaierAcYrw02Heat
  #define IR_SEND_MODE_HEAT_SWING_V kHaierAcYrw02SwingVBottom
  #define IR_SEND_HAS_TOGGLE_LIGHT false
#elif APP_ENV == APP_ENV_OTHER
  #define APP_IR_TRANSMITTER false

  #define PIN_TEMP_SENSOR 10
  #define PIN_IR_TRANSMITTER 9
  #define PIN_IR_RECEIVER 5
  #define PIN_SD_SCK 36
  #define PIN_SD_MISO 37
  #define PIN_SD_MOSI 35
  #define PIN_SD_SS 38

  #define DHT_TYPE DHT11

/// /.pio -> ir_Haier.cpp -> IRHaierAC160::IRHaierAC160
  #define IR_SEND_CLASS IRHaierAC160
  #define IR_SEND_FAN kHaierAcYrw02FanHigh
  #define IR_SEND_MODE_COOL kHaierAcYrw02Cool
  #define IR_SEND_MODE_COOL_SWING_V kHaierAc160SwingVTop
  #define IR_SEND_MODE_HEAT kHaierAcYrw02Heat
  #define IR_SEND_MODE_HEAT_SWING_V kHaierAc160SwingVLow
  #define IR_SEND_HAS_TOGGLE_LIGHT false
#endif
