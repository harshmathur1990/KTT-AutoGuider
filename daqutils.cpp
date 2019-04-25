#ifndef DAQUTILS_H
#define DAQUTILS_H
#endif

#include <iostream>
#include "NIDAQmx.h"

extern double VoltageData[1];
extern TaskHandle DAQHandle;

int initDAQ();
int setVoltages();


int initDAQ(){
    std::cout << "Creating DAQ handle : " << DAQmxCreateTask("", &DAQHandle) << std::endl;
    std::cout << "Creating Analog Out : " << DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
                                                                          0.3, 2.3, DAQmx_Val_Volts, "") << std::endl;
    std::cout << "Initializing the DAQ handle : " << DAQmxStartTask(DAQHandle) << std::endl;

    return 0;
}


int setVoltages(){
    VoltageData[0] = static_cast<double>(20.0)/15.0;
    std::cout << "Applying voltage for X : " << DAQmxWriteAnalogF64(DAQHandle, 1, 1, 0.0, DAQmx_Val_GroupByChannel, VoltageData, NULL, NULL) << std::endl;
    return 0;
}