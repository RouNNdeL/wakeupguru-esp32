//
// Created by Krzysiek on 2019-11-02.
//

#ifndef WAKEUPGURU_TIME_UTILS_H
#define WAKEUPGURU_TIME_UTILS_H

#include <stdint.h>
#include <sys/time.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct {
    uint16_t year;
    uint8_t month;
    uint8_t date;
    uint8_t hours;
    uint8_t minutes;
    uint8_t seconds;
    uint8_t w_day;
    uint8_t milis_256;
    uint8_t reason;
} __attribute__((__packed__)) ble_time;

void ble_time_to_timeval(struct timeval *tv, ble_time *ble);

void tm_to_ble_time(ble_time *time, struct tm *tm, uint8_t millis256) ;

void tm_to_timeval(struct timeval *tv, struct tm *tm, suseconds_t usec);

void timeval_to_tm(struct tm **tm, struct timeval *tv);

void ble_time_to_tm(struct tm *tm, ble_time *time);

void timeval_to_ble(ble_time *ble, struct timeval *tv);


#define BLE_TIME_REASON_MANUAL_UPDATE (1 << 0)
#define BLE_TIME_REASON_EXT_REF_UPDATE (1 << 1)
#define BLE_TIME_REASON_TZ_CHANGE (1 << 2)
#define BLE_TIME_REASON_DST_CHANGE (1 << 3)

#endif //WAKEUPGURU_TIME_UTILS_H
