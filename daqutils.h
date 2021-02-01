//
// Created by IIAP on 26/04/2019.
//

#ifndef FPVOLTAGECONTROLLER_DAQUTILS_H
#define FPVOLTAGECONTROLLER_DAQUTILS_H

#include <iostream>
#include <string>
#include <windows.h>
#include "NIDAQmx.h"

//extern double VoltageData[1];
extern TaskHandle DAQHandle;

int createDAQTask();
int clearDAQTask();
int startDAQTask();
int stopDAQTask();
//int initDAQAOVoltageChan();
int initDAQAIChan(const char deviceName[]);
//int setVoltages(double voltage);
int getVoltage(float64 *readArray, int32 *samplesReadPerChannel, int numberOfSamplesPerChannel, int readArraySize);
int testChannel(const char deviceName[]);
int monitorChannel(const char deviceName[], int numOfChannels);

#endif //FPVOLTAGECONTROLLER_DAQUTILS_H
