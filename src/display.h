#pragma once
#include "sensors.h"
#include "gps_manager.h"

void initDisplay();
void showData(const SensorData &data, const GPSData &gps);
void showMessage(const char* msg);
