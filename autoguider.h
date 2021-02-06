//
// Created by IIAP on 1/12/2021.
//

#ifndef FPVOLTAGECONTROLLER_AUTOGUIDER_H
#define FPVOLTAGECONTROLLER_AUTOGUIDER_H

#include <windows.h>
#include "daqutils.h"
#include "motorcontrols.h"
#include "utilheaders.h"

extern bool pauseLoop;

typedef struct {
    float64 raSlope, raConstant, raReferenceVoltage, decSlope, decConstant, decReferenceVoltage, loopUpdateTimeInSeconds, pollingTimeInMiliSeconds;
    int64 numberOfVoltageSamples, raFrequency, decFrequency, statusWorker, maxVoltageChangeInMiliVoltsPerSecRA, maxVoltageChangeInMiliVoltsPerSecDEC, mode
    ;
} worker_params;

int closedLoop();

DWORD WINAPI closedLoopThread(LPVOID lparam);

int calibrateRA();

int calibrateDEC();

int calibrateChannel(int motorNum, const char deviceName[], int mode);

int getMeanRAAndDEC(float64 *readArray, int64 readArraySize, int32 samplesReadPerChannel, int mode, float64 *meanRA, float64 *meanDEC);

int monitorRaAndDEC();

#endif //FPVOLTAGECONTROLLER_AUTOGUIDER_H
