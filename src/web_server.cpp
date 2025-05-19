#include "web_server.h"

#include <ArduinoJson.h>
#include <ESPAsyncWebServer.h>
#include <LittleFS.h>

#include "config.h"
#include "ir_control.h"
#include "status.h"

// HTTP 服务器
AsyncWebServer server(80);

void handleStatusRequest(AsyncWebServerRequest *request) {
    JsonDocument doc;
    doc["temp1"] = temp1;
    doc["temp2"] = temp2;
    doc["temp3"] = temp3;
    doc["ambient_temp"] = ambient_temp;
    doc["ambient_humidity"] = ambient_humidity;
    doc["cooling"] = coolingStatus;
    doc["pwm"] = pwmValue;

    String json;
    serializeJson(doc, json);
    request->send(200, "application/json", json);
}

void handleTECControl(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, data, len);
    if (error || !doc["status"].is<bool>()) {
        request->send(400, "application/json", R"({"error":"invalid payload"})");
        return;
    }

    coolingStatus = doc["status"];
    digitalWrite(PIN_TEC_CONTROL, coolingStatus ? HIGH : LOW);
    digitalWrite(PIN_FAN_CONTROL, coolingStatus ? HIGH : LOW);
    request->send(200, "application/json", R"({"result":true})");
}

void handleIRControl(AsyncWebServerRequest *request, uint8_t *data, size_t len, size_t, size_t) {
    JsonDocument doc;
    const DeserializationError error = deserializeJson(doc, data, len);
    if (error) {
        request->send(400, "application/json", R"({"error":"invalid JSON"})");
        return;
    }
    sendIR(doc);
    request->send(200, "application/json", R"({"result":true})");
}

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

void initWebServer() {
    if (!LittleFS.begin()) {
        Serial.println("LittleFS 初始化失败！");
        return;
    }

    server.on("/api/status", HTTP_GET, handleStatusRequest);

    server.on(
        "/api/tec_control", HTTP_POST,
        [](AsyncWebServerRequest *request) {
        }, // empty handler
        nullptr,
        handleTECControl
    );

    initIR();
    server.on(
        "/api/ir_control", HTTP_POST,
        [](AsyncWebServerRequest *request) {
        }, // empty handler
        nullptr,
        handleIRControl
    );

    // 静态页面加载
    server.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

    server.onNotFound(notFound);

    server.begin();
    Serial.println("HTTP服务已启动");
}
