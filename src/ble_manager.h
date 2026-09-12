#ifndef BLE_MANAGER_H
#define BLE_MANAGER_H

#include <Arduino.h>

void initBLE();
void sendBLEMessage(const String &message);
bool isBLEConnected();

#endif