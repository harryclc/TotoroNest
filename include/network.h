#pragma once

void connectWiFi();

void ensureWiFi();

bool isWiFiConnected();

void scanWiFi();

void connectMQTT();

void ensureMQTT();

bool isMQTTConnected();

bool publishAmbient(float temperature, int humidity);

bool publishTemp(int location, float temperature);

bool publishStatus(bool cooling, int pwm);
