#include "sensors.h"

#include <OneWire.h>
#include <DallasTemperature.h>

#include "config.h"
#include "status.h"

OneWire ow1(PIN_ONEWIRE_1), ow2(PIN_ONEWIRE_2), ow3(PIN_ONEWIRE_3);
DallasTemperature s1(&ow1), s2(&ow2), s3(&ow3);

bool tempRequested = false;
unsigned long tempRequestTime = 0;
unsigned long lastTempCycle = 0;

void initSensors() {
    s1.begin();
    s2.begin();
    s3.begin();
    s1.setWaitForConversion(false);
    s2.setWaitForConversion(false);
    s3.setWaitForConversion(false);
    s1.setResolution(12);
    s2.setResolution(12);
    s3.setResolution(12);
}

bool requestTemp() {
    const unsigned long now = millis();
    if (!tempRequested && now - lastTempCycle > TEMPERATURE_UPDATE_INTERVAL) {
        // 请求读取温度
        s1.requestTemperatures();
        s2.requestTemperatures();
        s3.requestTemperatures();
        tempRequestTime = now;
        tempRequested = true;
        return true;
    }
    return false;
}

bool readTemp() {
    const unsigned long now = millis();
    if (tempRequested && now - tempRequestTime > TEMPERATURE_CONVERSION_DELAY) {
        // 读取温度
        temp1 = s1.getTempCByIndex(0);
        temp2 = s2.getTempCByIndex(0);
        temp3 = s3.getTempCByIndex(0);

        tempRequested = false;
        lastTempCycle = now;

        return true;
    }
    return false;
}

bool isTempValid(float t) {
    if (!isnan(t) && t > TEMPERATURE_LOWER_LIMIT && t < TEMPERATURE_UPPER_LIMIT) {
        return true;
    }
    return false;
}
