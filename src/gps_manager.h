#pragma once

struct GPSData {
  double lat;
  double lng;
  double alt;
  int satellites;
  bool valid;
};

void initGPS();
GPSData readGPS();