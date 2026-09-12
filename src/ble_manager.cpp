#include "ble_manager.h"

#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

#define DEVICE_NAME "ESP32_JSON"

#define SERVICE_UUID        "00001234-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID "00005678-0000-1000-8000-00805f9b34fb"

BLECharacteristic *pCharacteristic;
bool deviceConnected = false;

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Cliente BLE conectado");
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Cliente BLE desconectado");

        BLEDevice::startAdvertising();
    }
};

void initBLE() {

    BLEDevice::init(DEVICE_NAME);
    // The JSON payload is larger than the default 20-byte ATT payload.
    BLEDevice::setMTU(64);

    BLEServer *pServer = BLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    BLEService *pService = pServer->createService(SERVICE_UUID);

    pCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID,
        BLECharacteristic::PROPERTY_NOTIFY
    );

    pCharacteristic->addDescriptor(new BLE2902());

    pCharacteristic->setValue("ESP32 iniciado");

    pService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();

    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);

    BLEDevice::startAdvertising();

    Serial.println("BLE iniciado");
}

void sendBLEMessage(const String &message) {

    if (!deviceConnected)
        return;

    pCharacteristic->setValue(message.c_str());
    pCharacteristic->notify();
    Serial.print("BLE TX: ");
    Serial.println(message);
}

bool isBLEConnected() {
    return deviceConnected;
}
