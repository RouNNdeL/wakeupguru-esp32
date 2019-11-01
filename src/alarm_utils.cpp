//
// Created by Krzysiek on 2019-10-25.
//

#include <WString.h>
#include <cstdio>
#include "config.h"
#include "alarm_utils.h"

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