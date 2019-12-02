//
// Created by Krzysiek on 2019-10-25.
//

#include <WString.h>
#include <cstdio>
#include <time.h>
#include "config.h"
#include "alarm_utils.h"

time_t get_next_alarm_entry(alarm_entry *alarm, tm current, alarm_entry *alarms, uint8_t count) {
    u_long closest = -1;
    for(int i = 0; i < count; ++i) {
        time_t t = get_next_alarm(current, alarms[i]);
        if(t < closest) {
            *alarm = alarms[i];
            closest = t;
        }
    }
    return closest;
}

time_t get_next_alarm(tm c_tm, alarm_entry alarm) {
    if(!(alarm.time_week_type & ALARM_WEEK_TYPE_EVEN) && !(alarm.time_week_type & ALARM_WEEK_TYPE_ODD)) {
        return -1;
    }

    time_t t0 = mktime(&c_tm);

    uint16_t time = alarm.time_week_type & ALARM_TIME_MASK;
    uint8_t minute = time % 60;
    uint8_t hour = time / 60;
    uint8_t current_week_day = c_tm.tm_wday ? c_tm.tm_wday - 1 : 6;
    uint result_weekday = 0;

    for(int i = 0; i < 7; ++i) {
        if(alarm.day_of_week & (1 << i)) {
            result_weekday = i;
            if(i > current_week_day ||
               (i == current_week_day && c_tm.tm_hour * 60 + c_tm.tm_min < time)) {
                break;
            }
        }
    }

    c_tm.tm_sec = 0;
    c_tm.tm_min = minute;
    c_tm.tm_hour = hour;

    time_t offset = (result_weekday - current_week_day) * 24 * 60 * 60;
    time_t t1 = mktime(&c_tm);
    t1 += offset;
    if(t1 < t0) {
        t1 += 7 * 24 * 60 * 60;
    }
    return t1;
}

String formatAlarmAsString(alarm_entry alarm) {
    String output = "";

    uint16_t time = alarm.time_week_type & ALARM_TIME_MASK;
    uint8_t hour = time / 60;
    uint8_t minute = time % 60;

    char *s_time = (char *) malloc(sizeof(char) * 5);
    sprintf(s_time, "%02d:%02d", hour, minute);
    output += String(s_time) + " - (";
    uint8_t space = 0;
    for(int i = 0; i < 7; ++i) {
        if(alarm.day_of_week & (1 << i)) {
            if(space) {
                output += " ";
            }

            space = 1;
            output += dayOfWeek(i);
        }
    }
    output += ") ";

    if((alarm.day_of_week & ALARM_WEEK_TYPE_ODD) && (alarm.day_of_week & ALARM_WEEK_TYPE_EVEN)) {
        output += "B";
    } else if(alarm.time_week_type & ALARM_WEEK_TYPE_EVEN) {
        output += "E";
    } else if(alarm.time_week_type & ALARM_WEEK_TYPE_ODD) {
        output += "O";
    } else {
        output = "No alarm";
    }

    return output;
}

String dayOfWeek(uint8_t day) {
    switch(day) {
        case 0:
            return "Mon";
        case 1:
            return "Tue";
        case 2:
            return "Wed";
        case 3:
            return "Thu";
        case 4:
            return "Fri";
        case 5:
            return "Sat";
        case 6:
            return "Sun";
        default:
            return "INVALID_DAY";
    }
}