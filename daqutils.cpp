


#include "daqutils.h"

int createDAQTask(){
    int status = DAQmxCreateTask("", &DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Creating DAQ handle : " << status_string << std::endl;
    return status;
}

int initialiseDAQHandle(){
    int status = DAQmxStartTask(DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Initializing the DAQ handle : " << status_string << std::endl;
    return  status;
}

int initDAQAOVoltageChan(){
    int status = DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
                                          -10, 10, DAQmx_Val_Volts, "");
    std::cout << "Creating Analog Out : " << status << std::endl;
    return status;
}

int initDAQAIChan(){
    int status_0 = DAQmxCreateAIVoltageChan(DAQHandle, "Dev1/ai0", "", DAQmx_Val_Diff, -5, 5, DAQmx_Val_Volts, NULL);
    std::string status_1_str = status_0 == 0?"True":"False";
    std::cout << "Creating Analog Input 0: " << status_1_str << std::endl;
    return status_0;
}

int setVoltages(double voltage){
    VoltageData[0] = voltage;
    int status = DAQmxWriteAnalogF64(DAQHandle, 1, 1, 0.0, DAQmx_Val_GroupByScanNumber, VoltageData, NULL, NULL);
    std::cout << "Applying voltage for X : " << status << std::endl;
    return status;
}

int getVoltage(float64 *readArray, int32 *samplesReadperChannel, int readArraySize){
    int status = DAQmxReadAnalogF64(DAQHandle, readArraySize, 1, DAQmx_Val_GroupByScanNumber, readArray, readArraySize, samplesReadperChannel, NULL);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Fetched voltage : " << status_str << std::endl;
    return status;
}