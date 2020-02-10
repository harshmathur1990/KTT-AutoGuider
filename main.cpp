#include <iostream>
#include <string>
#include <stdlib.h>
#include "daqutils.h"

double VoltageData[1];
TaskHandle DAQHandle;


int main() {
    std::cout << "Hello, World!" << std::endl;
    createDAQTask();
    initDAQAIChan();
    initialiseDAQHandle();
    std::string input;
    int numOfVoltageSamples;
    while (1) {
        std::cout << "Enter e for exit or Number of sample Voltages to get: " << std::endl;
        getline (std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            break;
        }
        numOfVoltageSamples = atoi(input.c_str());
        std::cout<<"The value of sample voltages to be get are: "<< numOfVoltageSamples<<std::endl;
        float64 *readArray = NULL;
        readArray = (float64*)calloc(numOfVoltageSamples, sizeof(float64));
        for (int i=0; i < numOfVoltageSamples; i++) {
            std::cout << "Initial Voltage[" << i << "] = " << readArray[i] << std::endl;
        }
        int32 samplesReadperChannel = 0;
        int status = getVoltage(readArray, &samplesReadperChannel, numOfVoltageSamples);
        std::cout<<"The status from reading input samples is "<<status<<std::endl;
        std::cout<<"The number of samples read are " << samplesReadperChannel << std::endl;
        if (status == 0) {
            for (int i=0; i < samplesReadperChannel; i++) {
                std::cout << "Voltage[" << i << "] = " << readArray[i] << std::endl;
            }
        }
    }
    return 0;
}