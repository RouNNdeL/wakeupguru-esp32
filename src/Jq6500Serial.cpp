//
// Created by Krzysiek on 2019-11-01.
//

#include "Jq6500Serial.h"


void Jq6500Serial::begin(ulong baud) {
    HardwareSerial::begin(baud, SERIAL_8N1, rx_pin, tx_pin);
}

void Jq6500Serial::play() {
    sendCommand(MP3_CMD_PLAY);
}

void Jq6500Serial::pause() {
    sendCommand(MP3_CMD_PAUSE);
}

void Jq6500Serial::sendCommand(uint8_t command) {
    write(MP3_CMD_BEGIN);
    write(0x02);
    write(command);
    write(MP3_CMD_END);
}

void Jq6500Serial::sendCommand(uint8_t command, uint8_t arg1) {
    write(MP3_CMD_BEGIN);
    write(0x03);
    write(command);
    write(arg1);
    write(MP3_CMD_END);
}

void Jq6500Serial::sendCommand(uint8_t command, uint8_t arg1, uint8_t arg2) {
    write(MP3_CMD_BEGIN);
    write(0x04);
    write(command);
    write(arg1);
    write(arg2);
    write(MP3_CMD_END);
}