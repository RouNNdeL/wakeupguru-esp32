//
// Created by Krzysiek on 2019-11-01.
//

#ifndef WAKEUPGURU_JQ6500SERIAL_H
#define WAKEUPGURU_JQ6500SERIAL_H


#include <cstdint>
#include <HardwareSerial.h>

class Jq6500Serial : public HardwareSerial {
public:
    Jq6500Serial(int uartNr, uint8_t txPin, uint8_t rxPin)
            : HardwareSerial(uartNr),
              tx_pin(txPin),
              rx_pin(rxPin) {};

    Jq6500Serial(uint8_t txPin, uint8_t rxPin)
            : HardwareSerial(1),
              tx_pin(txPin),
              rx_pin(rxPin) {};

    void begin(ulong baud = 9600);

    void play();

    void pause();

private:
    uint8_t tx_pin;
    uint8_t rx_pin;

    void sendCommand(uint8_t command);

    void sendCommand(uint8_t command, uint8_t arg1);

    void sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2);
};

static const uint8_t MP3_CMD_BEGIN = 0x7E;
static const uint8_t MP3_CMD_END   = 0xEF;

static const uint8_t MP3_CMD_PLAY = 0x0D;
static const uint8_t MP3_CMD_PAUSE = 0x0E;
static const uint8_t MP3_CMD_NEXT = 0x01;
static const uint8_t MP3_CMD_PREV = 0x02;
static const uint8_t MP3_CMD_PLAY_IDX = 0x03;

static const uint8_t MP3_CMD_NEXT_FOLDER = 0x0F;
static const uint8_t MP3_CMD_PREV_FOLDER = 0x0F; // Note the same as next, the data byte indicates direction

static const uint8_t MP3_CMD_PLAY_FILE_FOLDER = 0x12;

static const uint8_t MP3_CMD_VOL_UP = 0x04;
static const uint8_t MP3_CMD_VOL_DN = 0x05;
static const uint8_t MP3_CMD_VOL_SET = 0x06;

static const uint8_t MP3_CMD_EQ_SET = 0x07;
static const uint8_t MP3_CMD_LOOP_SET = 0x11;
static const uint8_t MP3_CMD_SOURCE_SET = 0x09;
static const uint8_t MP3_CMD_SLEEP = 0x0A;
static const uint8_t MP3_CMD_RESET = 0x0C;

static const uint8_t MP3_CMD_STATUS = 0x42;
static const uint8_t MP3_CMD_VOL_GET = 0x43;
static const uint8_t MP3_CMD_EQ_GET = 0x44;
static const uint8_t MP3_CMD_LOOP_GET = 0x45;
static const uint8_t MP3_CMD_VER_GET = 0x46;

static const uint8_t MP3_CMD_COUNT_SD  = 0x47;
static const uint8_t MP3_CMD_COUNT_MEM = 0x49;
static const uint8_t MP3_CMD_COUNT_FOLDERS = 0x53;
static const uint8_t MP3_CMD_CURRENT_FILE_IDX_SD = 0x4B;
static const uint8_t MP3_CMD_CURRENT_FILE_IDX_MEM = 0x4D;

static const uint8_t MP3_CMD_CURRENT_FILE_POS_SEC = 0x50;
static const uint8_t MP3_CMD_CURRENT_FILE_LEN_SEC = 0x51;
static const uint8_t MP3_CMD_CURRENT_FILE_NAME = 0x52;

#endif //WAKEUPGURU_JQ6500SERIAL_H
