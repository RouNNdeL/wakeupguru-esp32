//
// Created by Krzysiek on 2019-10-25.
//

#ifndef WAKEUPGURU_CONFIG_H
#define WAKEUPGURU_CONFIG_H

#define VERSION_CODE 1
#define VERSION_NAME "0.0.1"
#define BUILD_DATE (String(__TIME__) + "@" + __DATE__)

#define MAX_ALARM_COUNT 12

#define PIN_AUDIO_OFF 23
#define PIN_AUDIO_RX 19
#define PIN_AUDIO_TX 18
#define PIN_LED 13
#define PIN_VIBRATOR 14
#define PIN_OLED_SCL 4
#define PIN_OLED_SCK 16

#define LED_PWM_FREQ 20000
#define LED_PWM_CHANNEL 0

#define VIBRATOR_PWM_FREQ 20000
#define VIBRATOR_PWM_CHANNEL 1

#ifndef BLE_NAME
#define BLE_NAME "WakeUpGuru"
#endif

#define BLE_SERVICE_ALARM_UUID "bb09bd7e-4a65-4f20-b999-f7ca2537b5d4"
#define BLE_CHARACTERISTIC_ALARM_UUID "7322959e-fe38-4a5d-b60d-ed183beefd19"

#define BLE_SERVICE_CONTROL_UUID "e9a6f150-ca07-40f5-af49-f42344e2b074"
#define BLE_CHARACTERISTIC_CONTROL_UUID "496d8956-a90e-49ec-bf45-7cac6fe37868"

#ifdef SERIAL_DEBUG
#define PRINTLN(d) Serial.println(d)
#define PRINT(d) Serial.print(d)
#else
#define PRINTLN(d)
#define PRINT(d)
#endif

#endif //WAKEUPGURU_CONFIG_H
