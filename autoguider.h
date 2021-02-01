//
// Created by IIAP on 1/12/2021.
//

#ifndef FPVOLTAGECONTROLLER_AUTOGUIDER_H
#define FPVOLTAGECONTROLLER_AUTOGUIDER_H

#include <windows.h>
#include "daqutils.h"
#include "motorcontrols.h"

extern bool runClosedLoop;

int closedLoop();

DWORD WINAPI closedLoopThread(LPVOID lparam);

int calibrateRA();

int calibrateDEC();

int calibrateChannel(int motorNum, const char deviceName[]);

typedef struct {
    float64 raSlope, raConstant, raReferenceVoltage, decSlope, decConstant, decReferenceVoltage;
    int64 numberOfVoltageSamples, raFrequency, decFrequency, statusWorker, loopUpdateTimeInSeconds, maxVoltageChangeInMiliVoltsPerSecRA, maxVoltageChangeInMiliVoltsPerSecDEC
    ;
} worker_params;

template <typename T> int sgn(T val) {
    return (T(0) < val) - (val < T(0));
}

int monitorRaAndDEC();

#endif //FPVOLTAGECONTROLLER_AUTOGUIDER_H
