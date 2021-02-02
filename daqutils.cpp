


#include "daqutils.h"

TaskHandle DAQHandle;


int createDAQTask(){
    int status = DAQmxCreateTask("", &DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Creating DAQ handle : " << status_string << std::endl;
    return status;
}

int clearDAQTask() {
    int status = DAQmxClearTask(DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Clearing DAQ handle : " << status_string << std::endl;
    return status;
}

int startDAQTask(){
    int status = DAQmxStartTask(DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Initializing the DAQ handle : " << status_string << std::endl;
    return  status;
}

int stopDAQTask() {
    int status = DAQmxStopTask(DAQHandle);
    std::string status_string = status == 0?"True":"False";
    std::cout << "Stop Task of the DAQ handle : " << status_string << std::endl;
    return  status;
}

//int initDAQAOVoltageChan(){
//    int status = DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
//                                          -10, 10, DAQmx_Val_Volts, "");
//    std::cout << "Creating Analog Out : " << status << std::endl;
//    return status;
//}

int initDAQAIChan(const char deviceName[]){
    int status_0 = DAQmxCreateAIVoltageChan(DAQHandle, deviceName, "", DAQmx_Val_Diff, -5, 5, DAQmx_Val_Volts, NULL);
//    int status_0 = DAQmxCreateAIVoltageChan(DAQHandle, "Dev1/ai0", "", DAQmx_Val_Diff, -5, 5, DAQmx_Val_Volts, NULL);
    std::string status_1_str = status_0 == 0?"True":"False";
    std::cout << "Creating Analog Input: " << status_1_str << std::endl;
    return status_0;
}

//int setVoltages(double voltage){
//    VoltageData[0] = voltage;
//    int status = DAQmxWriteAnalogF64(DAQHandle, 1, 1, 0.0, DAQmx_Val_GroupByScanNumber, VoltageData, NULL, NULL);
//    std::cout << "Applying voltage for X : " << status << std::endl;
//    return status;
//}

int getVoltage(float64 *readArray, int32 *samplesReadPerChannel, int numberOfSamplesPerChannel, int readArraySize){
    int status = DAQmxReadAnalogF64(DAQHandle, numberOfSamplesPerChannel, 1, DAQmx_Val_GroupByScanNumber, readArray, readArraySize, samplesReadPerChannel, NULL);
    std::string status_str = status == 0?"True":"False";
    if (status != 0) {
        std::cout << "Fetched voltage : " << status_str << std::endl;
    }
    return status;
}

int monitorChannel(const char deviceName[], int numOfChannels) {
    std::string totalTimeInSecondsString, stepInMiliSecondsString, numberOfSamplesString;
    std::cout<<"How many seconds data to collect:"<<std::endl;
    getline (std::cin, totalTimeInSecondsString);
    int totalTimeInSeconds = atoi(totalTimeInSecondsString.c_str());
    std::cout<<"Enter the interval between each step in miliseconds:"<<std::endl;
    getline (std::cin, stepInMiliSecondsString);
    int stepInMiliSeconds = atoi(stepInMiliSecondsString.c_str());

    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        return -1;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName);
    if (statusInitDaQChain != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        clearDAQTask();
        return -2;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::cout<<"Failed to Start DaQ Task"<<std::endl;
        clearDAQTask();
        return -3;
    }
    std::cout<<"Enter number of samples to take:"<<std::endl;
    getline (std::cin, numberOfSamplesString);
    int numberOfSamples = atoi(numberOfSamplesString.c_str());
    int elapsedTime = 0;
    float64 *readArray = NULL;
    int32 samplesReadPerChannel = 0;
    while(elapsedTime < totalTimeInSeconds*1000) {
        readArray = (float64*)calloc( numberOfSamples * numOfChannels, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, numberOfSamples, numberOfSamples * numOfChannels);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            stopDAQTask();
            clearDAQTask();
            free(readArray);
            return -7;
        }
        std::cout<<elapsedTime;
        for (int i=0; i <  samplesReadPerChannel * numOfChannels; i++) {
            std::cout << "  " << readArray[i] * 1000;
        }
        std::cout<<std::endl;
        Sleep(stepInMiliSeconds);
        elapsedTime += stepInMiliSeconds;
        free(readArray);
    }
    std::cout<<"Data Recorded Successfully."<<std::endl;
    stopDAQTask();
    clearDAQTask();
    return 0;
}

int testChannel(const char deviceName[]) {
    int status;
    status = createDAQTask();
    if (status != 0) {
        return -1;
    }
    status = initDAQAIChan(deviceName);
    if (status != 0) {
        clearDAQTask();
        return -2;
    }
    status = startDAQTask();
    if (status != 0) {
        return -3;
    }
    std::string input;
    int numOfVoltageSamples;
    int reading;
    int i;
    char *token = NULL;
    while (1) {
        std::cout << "Enter e for exit or m to switch mode or Number of sample Voltages to get with the reading: " << std::endl;
        getline (std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            stopDAQTask();
            clearDAQTask();
            exit(0);
        }
        if (input[0] == 'm' || input[0] == 'M') {
            break;
        }
        token = std::strtok(const_cast<char *>(input.c_str()), " ");
        i = 0;
        reading = 0;
        while (token != NULL && i < 2) {
            switch (i) {
                case 0:
                    numOfVoltageSamples = atoi(token);
                    break;
                case 1:
                    reading = atoi(token);
                    break;
                default:
                    break;
            }
            token = std::strtok(NULL, " ");
            i += 1;
        }
        float64 *readArray = NULL;
        readArray = (float64*)calloc( numOfVoltageSamples, sizeof(float64));
        int32 samplesReadPerChannel = 0;
        int status = getVoltage(readArray, &samplesReadPerChannel, numOfVoltageSamples, numOfVoltageSamples);
        if (status == 0) {
            std::cout<<"Number of samples per channel: " << samplesReadPerChannel << std::endl;
            std::cout<<"Reading: " << reading << std::endl;
            for (int i=0; i <  samplesReadPerChannel; i++) {
                std::cout << "Voltage[" << 0 << "] = " << readArray[i] * 1000 << " mVolts"<< std::endl;
            }
        }
        free(readArray);
    }

    status = stopDAQTask();
    if (status != 0) {
        return -4;
    }
    status = clearDAQTask();
    if (status != 0) {
        return -5;
    }
    return 0;
}