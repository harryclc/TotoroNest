#include "status.h"

#include <cmath>

float temp1 = NAN, temp2 = NAN, temp3 = NAN;

float ambient_temp = NAN;
int ambient_humidity = 0;

bool coolingStatus = false;
int pwmValue = 0;
