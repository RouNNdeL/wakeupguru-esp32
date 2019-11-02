#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <Jq6500Serial.h>
#include <U8g2lib.h>
#include <sys/time.h>
#include "config.h"
#include "alarm_utils.h"

extern "C" {
#include "time_utils.h"
}

alarm_entry alarms[MAX_ALARM_COUNT];
uint8_t new_alarm;
Jq6500Serial mp3(PIN_AUDIO_TX, PIN_AUDIO_RX);
U8G2_SSD1306_128X32_UNIVISION_F_SW_I2C u8g2(U8G2_R2, PIN_OLED_SCK, PIN_OLED_SCL);

#define DISPLAY_MODE_COUNT 2

#define DISPLAY_MODE_STATUS 0
#define DISPLAY_MODE_TIME 1

uint8_t display_mode = DISPLAY_MODE_STATUS;

String display_status_lines[4] = {
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

            display_status_lines[2] = "LED: " + String(data[1] * 100 / 255) + "%";
            display_status_lines[3] = "Motor: " + String(data[2] * 100 / 255) + "%";

            if(data[0]) {
                display_status_lines[1] = "Audio: ON";
                digitalWrite(PIN_AUDIO_OFF, 1);
                delay(400);
                mp3.play();
            } else {
                display_status_lines[1] = "Audio: OFF";
                mp3.pause();
                delay(50);
                digitalWrite(PIN_AUDIO_OFF, 0);
            }
        }
    }
};

class CurrentTimeCharacteristicCallbacks : public BLECharacteristicCallbacks {
public:
    void onRead(BLECharacteristic *pCharacteristic) override {
        if(!strcmp(pCharacteristic->getUUID().toString().c_str(), BLE_CHARACTERISTIC_CURRENT_TIME_UUID)) {
            timeval tv;
            timezone tz;
            gettimeofday(&tv, &tz);

            ble_time time;
            timeval_to_ble(&time, &tv);
            time.reason = 0;
            PRINTLN("Hours: " + String(time.hours));
            PRINTLN("Minutes: " + String(time.minutes));

            pCharacteristic->setValue((uint8_t *) &time, sizeof(ble_time));
        }
    }

    void onWrite(BLECharacteristic *pCharacteristic) override {
        if(!strcmp(pCharacteristic->getUUID().toString().c_str(), BLE_CHARACTERISTIC_CURRENT_TIME_UUID)) {
            uint8_t *data = pCharacteristic->getData();
            ble_time t;
            memcpy(&t, data, sizeof(ble_time));

            timeval tv;
            ble_time_to_timeval(&tv, &t);

            timezone tz = {0, 0};
            settimeofday(&tv, &tz);

            Serial.println("Setting time of day");
            Serial.println(tv.tv_sec);
        }
    }
};

class ServerCallbacks : public BLEServerCallbacks {
public:
    void onConnect(BLEServer *pServer) override {
        PRINTLN("Device connected");
        display_status_lines[0] = "BT: Connected";
    }

    void onDisconnect(BLEServer *pServer) override {
        PRINTLN("Device disconnected");
        display_status_lines[0] = "BT: Disconnected";
    }
};

void change_display_mode() {
    display_mode = (display_mode + 1) % DISPLAY_MODE_COUNT;
}

void setup() {
    Serial.begin(115200);
    PRINTLN("|--------- Starting WakeUpGuru --------|");
    PRINTLN("|-- Build date: " + BUILD_DATE + " --|");
    PRINTLN("|-- Version Code: " + String(VERSION_CODE));
    PRINTLN("|-- Version Name: " + String(VERSION_NAME));
    PRINTLN("|-- BLE Name: " + String(BLE_NAME));

    pinMode(PIN_AUDIO_OFF, OUTPUT);
    pinMode(PIN_BTN, INPUT_PULLUP);

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

    BLEService *currentTimeService = bleServer->createService(BLE_SERVICE_CURRENT_TIME_UUID);
    BLECharacteristic *currentTimeCharacteristic = currentTimeService->createCharacteristic(
            BLE_CHARACTERISTIC_CURRENT_TIME_UUID,
            BLECharacteristic::PROPERTY_READ |
            BLECharacteristic::PROPERTY_WRITE
    );

    alarmCharacteristic->setCallbacks(new AlarmCharacteristicCallbacks());
    controlCharacteristic->setCallbacks(new ControlCharacteristicCallbacks());
    currentTimeCharacteristic->setCallbacks(new CurrentTimeCharacteristicCallbacks());

    bleServer->setCallbacks(new ServerCallbacks());

    alarmService->start();
    controlService->start();
    currentTimeService->start();

    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(BLE_SERVICE_ALARM_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x12);
    pAdvertising->setMinInterval(0x0128);
    pAdvertising->setMaxInterval(0x0160);
    BLEDevice::startAdvertising();

    mp3.begin();
    u8g2.begin();

    attachInterrupt(PIN_BTN, change_display_mode, FALLING);
}

void loop() {
    if(new_alarm) {
        new_alarm = 0;
        PRINTLN("Alarms in memory:");
        for(int i = 0; i < MAX_ALARM_COUNT; ++i) {
            PRINTLN(String(i)
                            +": " + formatAlarmAsString(alarms[i]));
        }
        PRINTLN("");
    }
    delay(10);

    switch(display_mode) {
        case DISPLAY_MODE_STATUS:
            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_pxplustandynewtv_8f);
            u8g2.drawStr(0, 7, display_status_lines[0].c_str());
            u8g2.drawStr(0, 16, display_status_lines[1].c_str());
            u8g2.drawStr(0, 24, display_status_lines[2].c_str());
            u8g2.drawStr(0, 32, display_status_lines[3].c_str());
            u8g2.sendBuffer();
            break;
        case DISPLAY_MODE_TIME:
            tm *time;
            timeval tv;
            timezone tz;

            gettimeofday(&tv, &tz);

            timeval_to_tm(&time, &tv);

            char d[30];
            char t[10];

            strftime(t, 10, "%H:%M:%S", time);
            strftime(d, 30, "%a, %b %d %Y", time);

            u8g2.clearBuffer();
            u8g2.setFont(u8g2_font_fub20_tn);
            u8g2.drawStr(8, 20, String(t).c_str());
            u8g2.setFont(u8g2_font_pxplustandynewtv_8r);
            u8g2.drawStr(0, 30, String(d).c_str());
            u8g2.sendBuffer();
            break;
    }
}