//
// Created by IIAP-IPC on 03/02/2021.
//

#ifndef FPVOLTAGECONTROLLER_UTILHEADERS_H
#define FPVOLTAGECONTROLLER_UTILHEADERS_H
#include <iostream>
#include <string>
#include <vector>
#define NORMALMODE 0
#define CLOUDMODE 1

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

int getModeAndDeviceName(std::string deviceOptions[], std::string *deviceName, int *mode);

#endif //FPVOLTAGECONTROLLER_UTILHEADERS_H
