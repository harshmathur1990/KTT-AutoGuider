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
//extern bool configExist;

typedef struct {
    float64 raSlope, raConstant, raReferenceVoltage, decSlope, decConstant, decReferenceVoltage, loopUpdateTimeInSeconds, pollingTimeInMiliSeconds, kp, ki, kd;
    int64 numberOfVoltageSamples, raFrequency, decFrequency, statusWorker, maxVoltageChangeInMiliVoltsPerSecRA, maxVoltageChangeInMiliVoltsPerSecDEC, mode
    ;
} worker_params;

int closedLoop();

DWORD WINAPI closedLoopThread(LPVOID lparam);

int calibrateRA();

int calibrateDEC();

int calibrateChannel(int motorNum, const char deviceName[], int mode);

int getMeanRAAndDEC(float64 *readArray, int64 readArraySize, int32 samplesReadPerChannel, float64 *meanRA,
                    float64 *meanDEC);

int monitorRaAndDEC();

int getCorrection(worker_params workerParams, float64 currMeanRA, float64 currMeanDEC, float64 refCountRA, float64 refCountDEC, float64 *prevErrorVoltageRA, float64 *integralErrorVoltageRA, float64 *curErrorVoltageRA, float64 *deltaErrorVoltageRA, float64 *prevErrorVoltageDEC, float64 *integralErrorVoltageDEC, float64 *curErrorVoltageDEC, float64 *deltaErrorVoltageDEC, int64 *raCorrection, int64 *decCorrection);
#endif //FPVOLTAGECONTROLLER_AUTOGUIDER_H
