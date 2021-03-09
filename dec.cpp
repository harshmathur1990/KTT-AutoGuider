//
// Created by harsh on 1/12/21.
//
#include "dec.h"


int testDec() {
    std::string deviceName = "Dev1/ai1";
    int numOfChannels = 1;
    testChannel(deviceName.c_str(), numOfChannels);
    return 0;
}

int monitorDEC() {
    std::string deviceName = "Dev1/ai1";
    int numOfChannels = 1;
    monitorChannel(deviceName.c_str(), numOfChannels);
    return 0;
}