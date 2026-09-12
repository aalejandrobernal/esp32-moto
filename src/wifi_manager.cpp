#include <Arduino.h>
#include <WiFi.h>
#include "wifi_manager.h"

const char* ssid = "venecia";
const char* password = "31283355";

bool initWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  Serial.print("Conectando WiFi");

  unsigned long inicio = millis();
  const unsigned long tiempoMaximo = 8000; // 8 segundos

  while (WiFi.status() != WL_CONNECTED && millis() - inicio < tiempoMaximo) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi OK");
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
    return true;
  }

  Serial.println("\nWiFi desconectado");
  return false;
}

bool isWiFiConnected() {
  return WiFi.status() == WL_CONNECTED;
}