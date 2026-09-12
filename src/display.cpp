#include "display.h"
#include <Arduino.h>
#include <U8g2lib.h>

U8G2_SH1106_128X64_NONAME_F_4W_SW_SPI u8g2(
    U8G2_R0, 18, 23, 5, 17, 16);

void initDisplay()
{
  Serial.begin(9600);
  u8g2.begin();
}

void showData(const SensorData &data, const GPSData &gps)
{

  // ===== SERIAL =====
  Serial.print("Temp: ");
  Serial.print(data.t_dht);
  Serial.print("C Hum: ");
  Serial.print(data.h);
  Serial.print("% Pres: ");
  Serial.print(data.presion);

  Serial.print(" I: ");
  Serial.print(data.corriente);
  Serial.print("A");

  Serial.print(" V: "); Serial.print(data.voltage, 2);
  Serial.print(" I226: "); Serial.print(data.inaCurrent, 3);
  Serial.print(" NTC: "); Serial.print(data.ntcTemperature, 1);
  Serial.print(" GPS: "); Serial.println(gps.valid ? "FIX" : "NO FIX");

  Serial.println();

  // ===== DISPLAY =====
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);

  char l1[25], l2[25], l3[25], l4[25];

  snprintf(l1, sizeof(l1), "T:%.1fC H:%.1f%%", data.t_dht, data.h);
  snprintf(l2, sizeof(l2),
        "P:%.0f I:%.2fA",
        data.presion,
        data.ina226Ok ? data.inaCurrent : data.corriente);

  snprintf(l3, sizeof(l3), "NTC:%.1fC V:%.2f", data.ntcTemperature, data.voltage);
  if (gps.valid) {
    snprintf(l4, sizeof(l4), "GPS S:%d %.2f %.2f", gps.satellites, gps.lat, gps.lng);
  } else {
    snprintf(l4, sizeof(l4), "GPS: buscando S:%d", gps.satellites);
  }

  u8g2.drawStr(0, 12, l1);
  u8g2.drawStr(0, 26, l2);
  u8g2.drawStr(0, 40, l3);
  u8g2.drawStr(0, 54, l4);

  u8g2.sendBuffer();
}
void showMessage(const char *msg)
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(0, 25, msg);
  u8g2.sendBuffer();

  Serial.println(msg);
}
