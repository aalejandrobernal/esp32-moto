#include <Arduino.h>
#include <TinyGPS++.h>
#include "gps_manager.h"

TinyGPSPlus gps;
HardwareSerial gpsSerial(1);

void initGPS() {
  // ATGM336H default output is NMEA at 9600 baud.
  gpsSerial.begin(9600, SERIAL_8N1, 26, 27); 
  // RX=26 recibe TX del GPS
  // TX=27 envia al RX del GPS
}

GPSData readGPS() {
  while (gpsSerial.available()) {
    gps.encode(gpsSerial.read());
  }

  GPSData data;
  data.valid = gps.location.isValid();
  data.lat = gps.location.lat();
  data.lng = gps.location.lng();
  data.alt = gps.altitude.meters();
  data.satellites = gps.satellites.value();

  return data;
}
