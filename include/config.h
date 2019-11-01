//
// Created by Krzysiek on 2019-10-25.
//

#ifndef WAKEUPGURU_CONFIG_H
#define WAKEUPGURU_CONFIG_H

#define VERSION_CODE 1
#define VERSION_NAME "0.0.1"
#define BUILD_DATE (String(__TIME__) + "@" + __DATE__)

#define MAX_ALARM_COUNT 12

#ifndef BLE_NAME
#define BLE_NAME "WakeUpGuru"
#endif

#define BLE_SERVICE_ALARM_UUID "bb09bd7e-4a65-4f20-b999-f7ca2537b5d4"
#define BLE_CHARACTERISTIC_ALARM_UUID "7322959e-fe38-4a5d-b60d-ed183beefd19"

#ifdef SERIAL_DEBUG
#define PRINTLN(d) Serial.println(d)
#define PRINT(d) Serial.print(d)
#else
#define PRINTLN(d)
#define PRINT(d)
#endif

#endif //WAKEUPGURU_CONFIG_H
