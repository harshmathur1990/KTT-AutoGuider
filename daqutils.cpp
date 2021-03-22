


#include "daqutils.h"

TaskHandle DAQHandle;


int createDAQTask(){
    int status = DAQmxCreateTask("", &DAQHandle);
    return status;
}

int clearDAQTask() {
    int status = DAQmxClearTask(DAQHandle);
    return status;
}

int startDAQTask(){
    int status = DAQmxStartTask(DAQHandle);
    return  status;
}

int stopDAQTask() {
    int status = DAQmxStopTask(DAQHandle);
    return  status;
}

//int initDAQAOVoltageChan(){
//    int status = DAQmxCreateAOVoltageChan(DAQHandle, "Dev1/ao0", "",
//                                          -10, 10, DAQmx_Val_Volts, "");
//    std::cout << "Creating Analog Out : " << status << std::endl;
//    return status;
//}

int initDAQAIChan(const char deviceName[]) {
    int32 terminalConfig = DAQmx_Val_Diff;
    int status_0 = DAQmxCreateAIVoltageChan(DAQHandle, deviceName, "", terminalConfig, -5, 5, DAQmx_Val_Volts, NULL);
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
    return status;
}

int monitorChannel(const char deviceName[], int numOfChannels) {
    std::string totalTimeInSecondsString, stepInMiliSecondsString, numberOfSamplesString;
    std::sprintf(logString, "How many seconds data to collect:");
    log(logString, PROMPT);
    getline (std::cin, totalTimeInSecondsString);
    int totalTimeInSeconds = atoi(totalTimeInSecondsString.c_str());
    std::sprintf(logString, "Enter the interval between each step in miliseconds:");
    log(logString, PROMPT);
    getline (std::cin, stepInMiliSecondsString);
    int stepInMiliSeconds = atoi(stepInMiliSecondsString.c_str());

    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::sprintf(logString, "Failed to Create DaQ Task");
        log(logString, ERROR);
        return -1;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName);
    if (statusInitDaQChain != 0) {
        std::sprintf(logString, "Failed to init DaQ chain");
        log(logString, ERROR);
        clearDAQTask();
        return -2;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::sprintf(logString, "Failed to Start DaQ Task.");
        log(logString, ERROR);
        clearDAQTask();
        return -3;
    }
    std::sprintf(logString, "Enter number of samples to take:");
    log(logString, PROMPT);
    getline (std::cin, numberOfSamplesString);
    int numberOfSamples = atoi(numberOfSamplesString.c_str());
    int elapsedTime = 0;
    float64 *readArray = NULL;
    int32 samplesReadPerChannel = 0;
    while(elapsedTime < totalTimeInSeconds*1000) {
        readArray = (float64*)calloc( numberOfSamples * numOfChannels, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, numberOfSamples, numberOfSamples * numOfChannels);
        if (statusReadVoltage != 0) {
            std::sprintf(logString, "Failed to get Voltage.");
            log(logString, ERROR);
            stopDAQTask();
            clearDAQTask();
            free(readArray);
            return -7;
        }
        char data[100000];
        char data_point[100];
        _itoa_s(elapsedTime, data, 10);
        for (int i=0; i <  samplesReadPerChannel * numOfChannels; i++) {
            std::sprintf(data_point, "  %.2f", readArray[i] * 1000);
            std::strcat(data, data_point);
        }
        log(data, INFO);
        Sleep(stepInMiliSeconds);
        elapsedTime += stepInMiliSeconds;
        free(readArray);
    }
    std::sprintf(logString, "Data Recorded Successfully.");
    log(logString, PROMPT);
    stopDAQTask();
    clearDAQTask();
    return 0;
}

int testChannel(const char *deviceName, int numOfChannels) {
    int64 readArraySize;
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
        std::sprintf(logString, "Enter e for exit or m to switch mode or Number of sample Voltages to get with the reading: ");
        log(logString, PROMPT);
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
        readArraySize = numOfVoltageSamples * numOfChannels;
        float64 *readArray = NULL;
        readArray = (float64*)calloc( readArraySize, sizeof(float64));
        int32 samplesReadPerChannel = 0;
        int status = getVoltage(readArray, &samplesReadPerChannel, numOfVoltageSamples, readArraySize);
        if (status == 0) {
            std::sprintf(logString, "Number of samples per channel: %d", samplesReadPerChannel);
            log(logString, INFO);
            std::sprintf(logString, "Reading: %d", reading);
            log(logString, INFO);
            for (int i=0; i <  readArraySize; i++) {
                std::sprintf(logString, "Voltage =  %.2f mVolts", readArray[i] * 1000);
                log(logString, INFO);
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