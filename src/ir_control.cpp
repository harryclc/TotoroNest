#include "ir_control.h"

#include <ArduinoJson.h>
#include <IRsend.h>
#include <IRutils.h>

#include "config.h"

IRsend irsend(PIN_IR_LED);

constexpr uint16_t stateLength = 23;
uint8_t remote_state[stateLength];

void stateReset() {
    for (unsigned char &i: remote_state) {
        i = 0x00;
    }
    remote_state[0] = 0x01;
    remote_state[1] = 0x10;
    remote_state[3] = 0x40;
    remote_state[5] = 0xFF;
    remote_state[7] = 0xE6;
    remote_state[9] = 0x89;
    remote_state[11] = 0x01;
    remote_state[13] = 0x3F;
    remote_state[19] = 0x01;
}

void initIR() {
    irsend.begin();
    stateReset();
}

void sendIR(JsonDocument json) {
    const String mode = json["mode"] | "cool";
    int temp = json["temperature"] | 26;
    const String fan = json["fan"] | "auto";

    if (mode == "off") {
        // 关机模式
        remote_state[21] = 0xA0;
    } else {
        // 其它模式下启用空调
        remote_state[21] = 0x90;
    }

    uint8_t _mode, _fan;
    if (mode == "cool") _mode = 0x06;
    else if (mode == "heat") _mode = 0x08;
    else if (mode == "fan") _mode = 0x02;
    else if (mode == "dry") _mode = 0x04;
    else if (mode == "auto") _mode = 0x0B;
    else _mode = 0xB;

    if (fan == "auto") _fan = 0b1000;
    else if (fan == "low") _fan = 0b0001;
    else if (fan == "medium") _fan = 0b0010;
    else if (fan == "high") _fan = 0b0100;
    else _fan = 0b1000;

    remote_state[17] = _fan << 4 | _mode & 0x0F;

    temp = constrain(temp, 16, 32);
    remote_state[15] = temp * 2 - 13;

    irutils::invertBytePairs(remote_state + 3, stateLength - 3);
    irsend.sendHitachiAc3(remote_state, stateLength);

    Serial.printf("[IR] Hitachi command: mode=%s temp=%d fan=%s\n", mode.c_str(), temp, fan.c_str());
}
