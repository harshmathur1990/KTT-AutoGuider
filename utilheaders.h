//
// Created by IIAP-IPC on 03/02/2021.
//

#ifndef FPVOLTAGECONTROLLER_UTILHEADERS_H
#define FPVOLTAGECONTROLLER_UTILHEADERS_H
#include <iostream>
#include <fstream>
#include <ctime>
#include <string>
#include <vector>
#define INDIVIDUAL 0
#define COUPLED 1
#define DEBUG 1
#define ERROR 0

extern int loggingMode;
extern std::string loggingFilename;
extern std::ofstream outfile;
extern char logString[1000];

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

int getModeAndDeviceName(std::string deviceOptions[], std::string *deviceName, int *mode);

int log(std::string logString, int severity);

int setupLogging();

#endif //FPVOLTAGECONTROLLER_UTILHEADERS_H
