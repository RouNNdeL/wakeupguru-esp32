#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include "config.h"
#include "alarm_utils.h"

alarm_entry alarms[MAX_ALARM_COUNT];
uint8_t new_alarm;

class AlarmCharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    void onRead(BLECharacteristic *pCharacteristic) override {
        PRINTLN(pCharacteristic->getUUID().toString().c_str() + String(" has been read from"));
    }

    void onWrite(BLECharacteristic *pCharacteristic) override {
        if(!strcmp(pCharacteristic->getUUID().toString().c_str(), BLE_CHARACTERISTIC_ALARM_UUID)) {
            uint8_t *data = pCharacteristic->getData();
            size_t size = pCharacteristic->getValue().length();
            memset(alarms, 0, sizeof(alarms));
            memcpy(alarms, data, min(sizeof(alarms), size));
            new_alarm = 1;
        }
    }
};

class ServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer *pServer) override {
        PRINTLN("Device connected");
    }

    void onDisconnect(BLEServer *pServer) override {
        PRINTLN("Device disconnected");
    }
};

void setup() {
    Serial.begin(115200);
    PRINTLN("|--------- Starting WakeUpGuru --------|");
    PRINTLN("|-- Build date: " + BUILD_DATE + " --|");
    PRINTLN("|-- Version Code: " + String(VERSION_CODE));
    PRINTLN("|-- Version Name: " + String(VERSION_NAME));
    PRINTLN("|-- BLE Name: " + String(BLE_NAME));

    BLEDevice::init(BLE_NAME);
    BLEServer *bleServer = BLEDevice::createServer();
    BLEService *alarmService = bleServer->createService(BLE_SERVICE_ALARM_UUID);
    BLECharacteristic *alarmCharacteristic = alarmService->createCharacteristic(
            BLE_CHARACTERISTIC_ALARM_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    alarmCharacteristic->setCallbacks(new AlarmCharacteristicCallbacks());
    bleServer->setCallbacks(new ServerCallbacks());

    alarmService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_ALARM_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMinInterval(0x0128);
    pAdvertising->setMaxInterval(0x0160);
    BLEDevice::startAdvertising();
}

void loop() {
    if(new_alarm) {
        new_alarm = 0;
        PRINTLN("Alarms in memory:");
        for(int i = 0; i < MAX_ALARM_COUNT; ++i) {
            PRINTLN(String(i) +": " + formatAlarmAsString(alarms[i]));
        }
        PRINTLN("");
    }
    delay(10);
}