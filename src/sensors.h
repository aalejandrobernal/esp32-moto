#pragma once

struct SensorData {
  float t_dht;
  float h;
  float t_bmp;
  float presion;
  float corriente;
  float voltage;
  float inaCurrent;
  float power;
  float ntcTemperature;
  bool ina226Ok;
};

void initSensors();
SensorData readSensors();
