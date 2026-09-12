#include "sensors.h"
#include <Arduino.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include <math.h>

#define DHTPIN 4
#define DHTTYPE DHT11

#define CURRENT_PIN 34
#define ACS_SENSITIVITY 0.066 

#define NTC_PIN 35
#define NTC_FIXED_RESISTOR 10000.0f
#define NTC_NOMINAL_RESISTANCE 10000.0f
#define NTC_NOMINAL_TEMPERATURE 25.0f
#define NTC_BETA 3950.0f
#define ADC_REFERENCE 3.3f

#define INA226_ADDRESS 0x40
#define INA226_SHUNT_RESISTANCE 0.1f
#define INA226_MAX_CURRENT 10.0f

float acsOffset = 0;
float inaCurrentLsb = 0;
bool ina226Ok = false;

void calibrarACS712();
float readCurrentDC();
bool initINA226();
void writeINA226Register(uint8_t reg, uint16_t value);
int16_t readINA226Signed(uint8_t reg);
uint16_t readINA226Unsigned(uint8_t reg);
float readNTCTemperature();

DHT dht(DHTPIN, DHTTYPE);
Adafruit_BMP280 bmp;

void initSensors() {
  dht.begin();

  pinMode(NTC_PIN, INPUT);
  analogSetPinAttenuation(NTC_PIN, ADC_11db);

  Wire.begin(21, 22);
  Wire.setClock(100000);

  bmp.begin(0x76);

  calibrarACS712();
  ina226Ok = initINA226();
}

SensorData readSensors() {
  SensorData data;

  data.h = dht.readHumidity();
  data.t_dht = dht.readTemperature();

  data.presion = bmp.readPressure() / 100.0;
  data.t_bmp = bmp.readTemperature();

  data.corriente = readCurrentDC();
  data.ina226Ok = ina226Ok;
  data.voltage = ina226Ok ? readINA226Unsigned(0x02) * 0.00125f : NAN;
  data.inaCurrent = ina226Ok ? readINA226Signed(0x04) * inaCurrentLsb : NAN;
  data.power = ina226Ok ? readINA226Signed(0x03) * (25.0f * inaCurrentLsb) : NAN;
  data.ntcTemperature = readNTCTemperature();

  return data;
}

bool initINA226() {
  Wire.beginTransmission(INA226_ADDRESS);
  if (Wire.endTransmission() != 0) return false;
  // 16-sample average, 1.1 ms conversions, continuous bus+shunt mode.
  writeINA226Register(0x00, 0x076F);
  inaCurrentLsb = INA226_MAX_CURRENT / 32768.0f;
  const uint16_t calibration = (uint16_t)(0.00512f /
      (inaCurrentLsb * INA226_SHUNT_RESISTANCE));
  writeINA226Register(0x05, calibration);
  return true;
}

void writeINA226Register(uint8_t reg, uint16_t value) {
  Wire.beginTransmission(INA226_ADDRESS);
  Wire.write(reg);
  Wire.write(value >> 8);
  Wire.write(value & 0xFF);
  Wire.endTransmission();
}

uint16_t readINA226Unsigned(uint8_t reg) {
  Wire.beginTransmission(INA226_ADDRESS);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0 || Wire.requestFrom((uint8_t)INA226_ADDRESS, (uint8_t)2) != 2) return 0;
  return ((uint16_t)Wire.read() << 8) | Wire.read();
}

int16_t readINA226Signed(uint8_t reg) {
  return (int16_t)readINA226Unsigned(reg);
}

float readNTCTemperature() {
  const int raw = analogRead(NTC_PIN);
  if (raw <= 0 || raw >= 4095) return NAN;
  const float voltage = raw * (ADC_REFERENCE / 4095.0f);
  const float resistance = NTC_FIXED_RESISTOR * voltage / (ADC_REFERENCE - voltage);
  const float kelvin = 1.0f / (1.0f / (NTC_NOMINAL_TEMPERATURE + 273.15f) +
      log(resistance / NTC_NOMINAL_RESISTANCE) / NTC_BETA);
  return kelvin - 273.15f;
}

void calibrarACS712() {
  long suma = 0;

  for (int i = 0; i < 500; i++) {
    suma += analogRead(CURRENT_PIN);
    delay(2);
  }

  float adcPromedio = suma / 500.0;
  acsOffset = adcPromedio * (3.3 / 4095.0);

  Serial.print("Offset ACS712: ");
  Serial.println(acsOffset, 4);
}

float readCurrentDC() {
  long suma = 0;

  for (int i = 0; i < 100; i++) {
    suma += analogRead(CURRENT_PIN);
    delay(2);
  }

  float adcPromedio = suma / 100.0;
  float voltaje = adcPromedio * (3.3 / 4095.0);

  float corriente = (voltaje - acsOffset) / ACS_SENSITIVITY;

  if (fabs(corriente) < 0.20) {
    corriente = 0;
  }

  return corriente;
}
