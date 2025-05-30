#pragma once

#define PIN_TEC_CONTROL 32
#define PIN_FAN_CONTROL 33
#define PIN_FAN_PWM     18
#define PIN_IR_LED      19
#define PIN_ONEWIRE_1   25
#define PIN_ONEWIRE_2   26
#define PIN_ONEWIRE_3   27

#define TEMPERATURE_LOWER_LIMIT (-50.0f)
#define TEMPERATURE_UPPER_LIMIT 100.0f
#define TEMPERATURE_CONVERSION_DELAY 750
#define TEMPERATURE_UPDATE_INTERVAL 250
#define TEMPERATURE_THRESHOLD_ON  28.0
#define TEMPERATURE_THRESHOLD_OFF 25.0

#define PWM_FREQUENCY  25000
#define PWM_RESOLUTION 8

#define BLE_SCAN_TIME_MS 5000
#define BLE_MI_SERVICE_UUID "ebe0ccb0-7a0a-4b0c-8a1a-6ff2997da3a6"
#define BLE_MI_TEMP_NOTIFY_CHAR_UUID "ebe0ccc1-7a0a-4b0c-8a1a-6ff2997da3a6"
