#pragma once

void initSensors();

bool requestTemp();

bool readTemp();

bool isTempValid(float t);
