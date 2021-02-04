//
// Created by harsh on 1/12/21.
//

#include "ra.h"

int testRa() {
    std::string deviceName;
    std::string deviceNameOptions[] = {"Dev1/ai2", "Dev1/ai2, Dev1/ai10"};
    int mode;
    getModeAndDeviceName(deviceNameOptions, &deviceName, &mode);
    testChannel(deviceName.c_str(), mode);
    return 0;
}

int monitorRA() {
    std::string deviceName;
    int numOfChannels;
    std::string deviceNameOptions[] = {"Dev1/ai2", "Dev1/ai2, Dev1/ai10"};
    int mode;
    getModeAndDeviceName(deviceNameOptions, &deviceName, &mode);
    if (mode == NORMALMODE)
        numOfChannels = 1;
    else
        numOfChannels = 2;
    monitorChannel(deviceName.c_str(), numOfChannels, mode);
    return 0;
}