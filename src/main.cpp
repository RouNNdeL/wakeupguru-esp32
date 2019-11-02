#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Jq6500Serial.h>
#include <U8g2lib.h>
#include <sys/time.h>
#include "config.h"
#include "alarm_utils.h"

alarm_entry alarms[MAX_ALARM_COUNT];
uint8_t new_alarm;
Jq6500Serial mp3(PIN_AUDIO_TX, PIN_AUDIO_RX);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R2, PIN_OLED_SCK, PIN_OLED_SCL);

String display_lines[4] = {
        "BT: Disconnected",
        "Audio: OFF",
        "LED: 0%",
        "Motor: 0%"
};

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

class ControlCharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    void onRead(BLECharacteristic *pCharacteristic) override {
        PRINTLN(pCharacteristic->getUUID().toString().c_str() + String(" has been read from"));
    }

    void onWrite(BLECharacteristic *pCharacteristic) override {
        if(!strcmp(pCharacteristic->getUUID().toString().c_str(), BLE_CHARACTERISTIC_CONTROL_UUID)) {
            uint8_t *data = pCharacteristic->getData();

            uint8_t brightness = data[1] * data[1] / UINT8_MAX;
            ledcWrite(LED_PWM_CHANNEL, brightness);
            ledcWrite(VIBRATOR_PWM_CHANNEL, data[2]);

            display_lines[2] = "LED: " + String(data[1] * 100 / 255) + "%";
            display_lines[3] = "Motor: " + String(data[2] * 100 / 255) + "%";

            if(data[0]) {
                display_lines[1] = "Audio: ON";
                digitalWrite(PIN_AUDIO_OFF, 1);
                delay(400);
                mp3.play();
            } else {
                display_lines[1] = "Audio: OFF";
                mp3.pause();
                delay(50);
                digitalWrite(PIN_AUDIO_OFF, 0);
            }
        }
    }
};

class ServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer *pServer) override {
        PRINTLN("Device connected");
        display_lines[0] = "BT: Connected";
    }

    void onDisconnect(BLEServer *pServer) override {
        PRINTLN("Device disconnected");
        display_lines[0] = "BT: Disconnected";
    }
};

void setup() {
    Serial.begin(115200);
    PRINTLN("|--------- Starting WakeUpGuru --------|");
    PRINTLN("|-- Build date: " + BUILD_DATE + " --|");
    PRINTLN("|-- Version Code: " + String(VERSION_CODE));
    PRINTLN("|-- Version Name: " + String(VERSION_NAME));
    PRINTLN("|-- BLE Name: " + String(BLE_NAME));

    pinMode(PIN_AUDIO_OFF, OUTPUT);

    digitalWrite(PIN_AUDIO_OFF, 0);

    ledcSetup(LED_PWM_CHANNEL, LED_PWM_FREQ, 8);
    ledcAttachPin(PIN_LED, LED_PWM_CHANNEL);
    ledcWrite(LED_PWM_CHANNEL, 0);

    ledcSetup(VIBRATOR_PWM_CHANNEL, VIBRATOR_PWM_FREQ, 8);
    ledcAttachPin(PIN_VIBRATOR, VIBRATOR_PWM_CHANNEL);
    ledcWrite(VIBRATOR_PWM_CHANNEL, 0);

    BLEDevice::init(BLE_NAME);
    BLEServer *bleServer = BLEDevice::createServer();
    BLEService *alarmService = bleServer->createService(BLE_SERVICE_ALARM_UUID);
    BLECharacteristic *alarmCharacteristic = alarmService->createCharacteristic(
            BLE_CHARACTERISTIC_ALARM_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    BLEService *controlService = bleServer->createService(BLE_SERVICE_CONTROL_UUID);
    BLECharacteristic *controlCharacteristic = controlService->createCharacteristic(
            BLE_CHARACTERISTIC_CONTROL_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    alarmCharacteristic->setCallbacks(new AlarmCharacteristicCallbacks());
    controlCharacteristic->setCallbacks(new ControlCharacteristicCallbacks());

    bleServer->setCallbacks(new ServerCallbacks());

    alarmService->start();
    controlService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_ALARM_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMinInterval(0x0128);
    pAdvertising->setMaxInterval(0x0160);
    BLEDevice::startAdvertising();

    mp3.begin();
    u8g2.begin();
}

void loop() {
    if(new_alarm) {
        new_alarm = 0;
        PRINTLN("Alarms in memory:");
        for(int i = 0; i < MAX_ALARM_COUNT; ++i) {
            PRINTLN(String(i) + ": " + formatAlarmAsString(alarms[i]));
        }
        PRINTLN("");
    }
    delay(10);

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_pxplustandynewtv_8f);
    u8g2.drawStr(0, 7, display_lines[0].c_str());
    u8g2.drawStr(0, 16, display_lines[1].c_str());
    u8g2.drawStr(0, 24, display_lines[2].c_str());
    u8g2.drawStr(0, 32, display_lines[3].c_str());
    u8g2.sendBuffer();
}