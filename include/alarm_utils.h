//
// Created by Krzysiek on 2019-10-25.
//

#ifndef WAKEUPGURU_ALARM_UTILS_H
#define WAKEUPGURU_ALARM_UTILS_H

#define ALARM_WEEK_TYPE_EVEN (1 << 14)
#define ALARM_WEEK_TYPE_ODD (1 << 15)

#define ALARM_TIME_MASK 0x07ff

/*
 * JavaScript Function to generate the values
   function gen_bytes(hour, minute, day, week) {
       return ((day).toString(16) + (hour*60+minute + (week << 14)).toString(16).match(/../g)
       .reverse().join('').toString(16)).padStart(6, 0);
   }
 */
typedef struct
{
    /*
     * Format:
     * b7 b6 b5 b4 b3 b2 b1 b0
     *
     * [b0-b6] - days of the week
     */
    uint8_t day_of_week;

    /*
     * Format:
     * bF bE bD bC bB bA b9 b8 b7 b6 b5 b4 b3 b2 b1 b0
     * 
     * [b0-bB] - hour * 60 + minute
     * bE - even week
     * bF - odd week
     * When both bE and bF are set then the alarm is triggered on both days
     */
    uint16_t time_week_type;
} __attribute__((packed)) alarm_entry;

String formatAlarmAsString(alarm_entry alarm);

String dayOfWeek(uint8_t day);

#endif //WAKEUPGURU_ALARM_UTILS_H
