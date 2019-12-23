#include "daqutils.h"

int createDAQTask(){
    int status = DAQmxCreateTask("", &DAQHandle);
    std::cout << "Creating DAQ handle : " << status << std::endl;
    return status;
}

int initialiseDAQHandle(){
    int status = DAQmxStartTask(DAQHandle);
    std::cout << "Initializing the DAQ handle : " << status << std::endl;
    return  status;
}

int initDAQAOVoltageChan(){
    int status = DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
                                          -10, 10, DAQmx_Val_Volts, "");
    std::cout << "Creating Analog Out : " << status << std::endl;
    return status;
}

int initDAQAIChan(){
    int status = DAQmxCreateAIVoltageChan(DAQHandle, "Dev1/ai0", "", DAQmx_Val_RSE, -5, 5, DAQmx_Val_Volts, NULL);
    std::cout << "Creating Analog Input : " << status << std::endl;
    return status;
}

int setVoltages(double voltage){
    VoltageData[0] = voltage;
    int status = DAQmxWriteAnalogF64(DAQHandle, 1, 1, 0.0, DAQmx_Val_GroupByChannel, VoltageData, NULL, NULL);
    std::cout << "Applying voltage for X : " << status << std::endl;
    return status;
}

int getVoltage(float64 *readArray, int readArraySize, int32 *samplesReadperChannel){
    int status = DAQmxReadAnalogF64(DAQHandle, readArraySize, 1, DAQmx_Val_GroupByChannel, readArray, readArraySize, samplesReadperChannel, NULL);
    std::cout << "Fetching voltage for X : " << status << std::endl;
    return status;
}