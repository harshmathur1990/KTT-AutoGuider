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

int closedLoopWorker();

int calibrateRA();

int calibrateDEC();

int calibrateChannel(int motorNum, const char deviceName[]);

#endif //FPVOLTAGECONTROLLER_AUTOGUIDER_H
