//
// Created by harsh on 1/12/21.
//

#include "ra.h"

int testRa() {
    std::string deviceName = "Dev1/ai2";
    int numOfChannels = 1;
    testChannel(deviceName.c_str(), numOfChannels);
    return 0;
}

int monitorRA() {
    std::string deviceName = "Dev1/ai2";
    int numOfChannels = 1;
    monitorChannel(deviceName.c_str(), numOfChannels);
    return 0;
}