#include "cooling.h"

#include <Arduino.h>
#include <driver/ledc.h>

#include "config.h"
#include "sensors.h"
#include "status.h"

void initCooling() {
    pinMode(PIN_TEC_CONTROL, OUTPUT);
    pinMode(PIN_FAN_CONTROL, OUTPUT);
    ledcSetup(LEDC_CHANNEL_0, PWM_FREQUENCY, PWM_RESOLUTION);
    ledcAttachPin(PIN_FAN_PWM, LEDC_CHANNEL_0);
}

void enableCooling() {
    digitalWrite(PIN_TEC_CONTROL, HIGH); // 开启 TEC
    digitalWrite(PIN_FAN_CONTROL, HIGH); // 开启风扇
    coolingStatus = true;
}

void disableCooling() {
    digitalWrite(PIN_TEC_CONTROL, LOW); // 关闭 TEC
    digitalWrite(PIN_FAN_CONTROL, LOW); // 关闭风扇
    coolingStatus = false;
}

void updateCooling() {
    if (isTempValid(temp1) && isTempValid(temp2)) {
        if (temp1 >= 27.5 || temp2 >= 27.5) {
            enableCooling();
        } else if (temp1 < 24.0 || temp2 < 25.0) {
            disableCooling();
        }
        if (temp1 > 30.0 || temp2 > 30.0) {
            pwmValue = 255;
        } else if (temp1 > 25.0 || temp2 > 25.0) {
            pwmValue = 128;
        } else {
            pwmValue = 64;
        }
        ledcWrite(LEDC_CHANNEL_0, pwmValue);
    } else if (isTempValid(ambient_temp)) {
        if (ambient_temp >= 27.5) {
            enableCooling();
        } else if (ambient_temp < 25.5) {
            disableCooling();
        }
        if (ambient_temp > 30.0) {
            pwmValue = 255;
        } else if (ambient_temp > 25.0) {
            pwmValue = 128;
        } else {
            pwmValue = 64;
        }
        ledcWrite(LEDC_CHANNEL_0, pwmValue);
    } else if (isTempValid(temp3)) {
        if (temp3 > 33.0) {
            enableCooling();
        } else if (temp3 < 31.0) {
            disableCooling();
        }
        if (temp3 > 35.0) {
            pwmValue = 255;
        } else if (temp3 > 30.0) {
            pwmValue = 128;
        } else {
            pwmValue = 64;
        }
        ledcWrite(LEDC_CHANNEL_0, pwmValue);
    }
}
