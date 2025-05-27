#include "network.h"

#include <WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

#include "secrets.h"
#include "sensors.h"

WiFiClient espClient;
PubSubClient mqttClient(MQTT_HOST, MQTT_PORT, espClient);

// 连接WiFi
void connectWiFi() {
    if (WiFiClass::status() != WL_CONNECTED) {
        WiFi.disconnect();
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        Serial.println("尝试连接WiFi...");
    }
}

// 检查WiFi状态
void ensureWiFi() {
    if (WiFiClass::status() != WL_CONNECTED) {
        static unsigned long lastAttempt = 0;
        if (millis() - lastAttempt > 10000) {
            lastAttempt = millis();
            Serial.println("WiFi未连接，正在尝试重新连接...");
            connectWiFi();
        }
    }
}

bool isWiFiConnected() {
    return mqttClient.connected();
}

// 扫描周围Wi-Fi网络
void scanWiFi() {
    Serial.println("开始扫描Wi-Fi...");
    const int networksFound = WiFi.scanNetworks(); // 执行Wi-Fi扫描
    Serial.println("扫描完成!");

    if (networksFound == 0) {
        Serial.println("未找到网络");
    } else {
        Serial.print("找到 ");
        Serial.print(networksFound);
        Serial.println(" 个网络：");

        for (int i = 0; i < networksFound; i++) {
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i)); // 打印SSID
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i)); // 打印信号强度
            Serial.print("dBm)");
            Serial.println(WiFi.encryptionType(i) == WIFI_AUTH_OPEN ? " 无密码" : " 有密码");
        }
    }
}

// 连接MQTT
void connectMQTT() {
    if (WiFiClass::status() == WL_CONNECTED) {
        Serial.print("IP地址: ");
        Serial.println(WiFi.localIP());
        if (!mqttClient.connected()) {
            // Serial.print("尝试连接到MQTT...");
            const String clientId = "ESP32Client-" + String(random(0xffff), HEX);
            if (mqttClient.connect(clientId.c_str(), MQTT_USER, MQTT_PASS)) {
                Serial.println("MQTT连接成功");
            } else {
                Serial.print("MQTT连接失败 rc=");
                Serial.println(mqttClient.state());
            }
        }
    }
}

// 检查MQTT连接状态
void ensureMQTT() {
    if (WiFiClass::status() == WL_CONNECTED) {
        if (!mqttClient.connected()) {
            static unsigned long lastMQTTAttempt = 0;
            if (millis() - lastMQTTAttempt > 5000) {
                lastMQTTAttempt = millis();
                connectMQTT();
            }
        } else {
            mqttClient.loop();
        }
    }
}

bool isMQTTConnected() {
    return mqttClient.connected();
}

// 推送数据
bool publish(const JsonDocument &doc, const char *topic = MQTT_TOPIC) {
    String payload;
    serializeJson(doc, payload);
    return mqttClient.publish(topic, payload.c_str());
}

// 推送环境温湿度数据
bool publishAmbient(const float temperature, const int humidity) {
    JsonDocument doc;
    doc["type"] = "ambient";
    doc["temperature"] = temperature;
    doc["humidity"] = humidity;
    return publish(doc);
}

// 推送传感器温度数据
bool publishTemp(const int location, const float temperature) {
    if (isTempValid(temperature)) {
        JsonDocument doc;
        doc["type"] = "temp";
        doc["location"] = location;
        doc["temperature"] = temperature;
        return publish(doc);
    }
    return false;
}

// 推送TEC状态
bool publishStatus(const bool cooling, const int pwm) {
    JsonDocument doc;
    doc["type"] = "tec";
    doc["cooling"] = cooling;
    doc["pwm"] = pwm;
    return publish(doc);
}
