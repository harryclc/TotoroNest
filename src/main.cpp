#include <Arduino.h>

#include "ble_mijia.h"
#include "cooling.h"
#include "network.h"
#include "oled_ui.h"
#include "sensors.h"
#include "status.h"
#include "web_server.h"

unsigned long lastReportTime = 0; // 定时器变量
constexpr unsigned long reportInterval = 10 * 1000; // 10秒

void setup() {
    // 初始化串口
    Serial.begin(115200);
    Serial.println("启动 TotoroNest Control");

    // 初始化温度传感器
    initSensors();
    // 初始化TEC和风扇控制
    initCooling();
    // 初始化屏幕
    initUI();
    // 连接网络和蓝牙
    connectWiFi();
    connectMQTT();
    initBLE();
    // 配置 HTTP 路由
    initWebServer();
}

void loop() {
    // 读取温度控制制冷
    requestTemp();
    if (readTemp()) {
        updateCooling();
    }

    // 屏幕显示
    drawUI();

    // 维护连接
    ensureWiFi();
    ensureMQTT();
    ensureBLE();

    // 定时发布数据
    if (millis() - lastReportTime >= reportInterval) {
        publishTemp(1, temp1);
        publishTemp(2, temp2);
        publishTemp(3, temp3);
        publishStatus(coolingStatus, pwmValue);

        lastReportTime = millis();
    }

    delay(10);
}
