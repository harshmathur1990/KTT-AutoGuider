//
// Created by IIAP on 26/04/2019.
//

#ifndef FPVOLTAGECONTROLLER_DAQUTILS_H
#define FPVOLTAGECONTROLLER_DAQUTILS_H

#include <iostream>
#include "NIDAQmx.h"

extern double VoltageData[1];
extern TaskHandle DAQHandle;

int initDAQ();
int setVoltages(double voltage);

#endif //FPVOLTAGECONTROLLER_DAQUTILS_H
