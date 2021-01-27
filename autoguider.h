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
    float64 slope, constant, referenceVoltage;
    int64 numberOfVoltageSamples, frequency, statusWorker, loopUpdateTimeInSeconds, maxVoltageChangeInMiliVoltsPerSec;
} worker_params;
#endif //FPVOLTAGECONTROLLER_AUTOGUIDER_H
