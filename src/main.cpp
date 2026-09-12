#include <Arduino.h>
#include "sensors.h"
#include "display.h"
#include "wifi_manager.h"
#include "gps_manager.h"
#include "ble_manager.h"

bool wifiOk = false;

// RPM remains zero until a tachometer or Hall sensor is connected.
int readRPM() {
  return 0;
}

void setup() {
  
  initDisplay();
  showMessage("Iniciando...");

  initSensors();

  showMessage("Conectando WiFi...");
  wifiOk = initWiFi();

  if (wifiOk) {
    showMessage("WiFi OK");
  } else {
    showMessage("WiFi desconectado");
  }
  initGPS();
  initBLE();

  delay(1500);
}

void loop() {
  SensorData data = readSensors();
  GPSData gps = readGPS();

  showData(data, gps);

  static unsigned long lastBLEMessage = 0;
  if (isBLEConnected() && (lastBLEMessage == 0 || millis() - lastBLEMessage >= 2000)) {
    // JSON never contains NaN: unavailable sensor values are reported as 0.
    const float temperature = isfinite(data.ntcTemperature) ? data.ntcTemperature : 0.0f;
    const float voltage = isfinite(data.voltage) ? data.voltage : 0.0f;
    const double latitude = gps.valid ? gps.lat : 0.0;
    const double longitude = gps.valid ? gps.lng : 0.0;
    const double altitude = gps.valid ? gps.alt : 0.0;

    const String message = "{\"tmp\":" + String(temperature, 1) +
      ",\"vol\":" + String(voltage, 1) +
      ",\"rpm\":" + String(readRPM()) +
      ",\"gps_valid\":" + String(gps.valid ? "true" : "false") +
      ",\"lat\":" + String(latitude, 6) +
      ",\"lng\":" + String(longitude, 6) +
      ",\"alt\":" + String(altitude, 1) +
      ",\"satellites\":" + String(gps.valid ? gps.satellites : 0) + "}";
    sendBLEMessage(message);
    lastBLEMessage = millis();
  }

  delay(100);
}
