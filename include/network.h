#pragma once

void connectWiFi();

void ensureWiFi();

bool isWiFiConnected();

void scanWiFi();

void connectMQTT();

void ensureMQTT();

bool isMQTTConnected();

void publishAmbient(float temperature, int humidity);

void publishTemp(int location, float temperature);

void publishStatus(bool cooling, int pwm);
