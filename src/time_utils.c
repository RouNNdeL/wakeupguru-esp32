//
// Created by Krzysiek on 2019-11-02.
//

#include "time_utils.h"

void ble_time_to_tm(struct tm *tm, ble_time *time) {
    tm->tm_year = time->year - 1900;
    tm->tm_mon = time->month - 1;
    tm->tm_mday = time->date;
    tm->tm_hour = time->hours;
    tm->tm_min = time->minutes;
    tm->tm_sec = time->seconds;
    tm->tm_wday = time->w_day % 7;
}

void tm_to_ble_time(ble_time *time, struct tm *tm, uint8_t millis256) {
    time->year = tm->tm_year + 1900;
    time->month = tm->tm_mon + 1;
    time->date = tm->tm_mday;
    time->hours = tm->tm_hour;
    time->minutes = tm->tm_min;
    time->seconds = tm->tm_sec;
    time->w_day = tm->tm_wday ? tm->tm_wday : 7;
    time->milis_256 = millis256;
}

void tm_to_timeval(struct timeval *tv, struct tm *tm, suseconds_t usec) {
    tv->tv_sec = mktime(tm);
    tv->tv_usec = usec;
}

void timeval_to_tm(struct tm **tm, struct timeval *tv) {
    time_t sec = tv->tv_sec;
    *tm = gmtime(&sec);
}

void ble_time_to_timeval(struct timeval *tv, ble_time *ble) {
    struct tm tm;
    ble_time_to_tm(&tm, ble);
    tm_to_timeval(tv, &tm, ble->milis_256 * 1000000 / UINT8_MAX);
}

void timeval_to_ble(ble_time *ble, struct timeval *tv) {
    struct tm *tm;
    timeval_to_tm(&tm, tv);
    tm_to_ble_time(ble, tm, tv->tv_usec * UINT8_MAX / 1000000);
}
