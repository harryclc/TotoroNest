#include "ir_control.h"

#include <ArduinoJson.h>
#include <IRremoteESP8266.h>
#include <ir_Hitachi.h>

#include "config.h"

IRHitachiAc1 ac(PIN_IR_LED);

void initIR() {
    ac.begin();
}

void sendIR(JsonDocument json) {
    const String mode = json["mode"] | "cool";
    int temp = json["temperature"] | 26;
    const String fan = json["fan"] | "auto";

    // 关机模式
    if (mode == "off") {
        ac.setPower(false);
        ac.send();
        Serial.println("[IR] 空调已关闭");
        return;
    }

    // 其它模式下启用空调
    ac.setPower(true);

    if (mode == "cool") ac.setMode(kHitachiAcCool);
    else if (mode == "heat") ac.setMode(kHitachiAcHeat);
    else if (mode == "fan") ac.setMode(kHitachiAcFan);
    else if (mode == "dry") ac.setMode(kHitachiAcDry);
    else if (mode == "auto") ac.setMode(kHitachiAcAuto);
    else ac.setMode(kHitachiAcCool);

    temp = constrain(temp, 16, 32);
    ac.setTemp(temp);

    if (fan == "auto") ac.setFan(kHitachiAcFanAuto);
    else if (fan == "low") ac.setFan(kHitachiAcFanLow);
    else if (fan == "medium") ac.setFan(kHitachiAcFanMed);
    else if (fan == "high") ac.setFan(kHitachiAcFanHigh);
    else ac.setFan(kHitachiAcFanAuto);

    ac.send();
    Serial.printf("[IR] Hitachi command: mode=%s temp=%d fan=%s\n", mode.c_str(), temp, fan.c_str());
}
