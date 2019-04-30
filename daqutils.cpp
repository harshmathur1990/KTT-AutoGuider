#include "daqutils.h"


int initDAQ(){
    std::cout << "Creating DAQ handle : " << DAQmxCreateTask("", &DAQHandle) << std::endl;
    std::cout << "Creating Analog Out : " << DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
                                                                          -10, 10, DAQmx_Val_Volts, "") << std::endl;
    std::cout << "Initializing the DAQ handle : " << DAQmxStartTask(DAQHandle) << std::endl;

    return 0;
}


int setVoltages(double voltage){
    VoltageData[0] = voltage;
    std::cout << "Applying voltage for X : " << DAQmxWriteAnalogF64(DAQHandle, 1, 1, 0.0, DAQmx_Val_GroupByChannel, VoltageData, NULL, NULL) << std::endl;
    return 0;
}