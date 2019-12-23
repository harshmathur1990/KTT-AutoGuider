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
        float64 *readArray = (float64*)malloc(numOfVoltageSamples * sizeof(float64));
        int32 *samplesReadperChannel;
        int status = getVoltage(&readArray, numOfVoltageSamples, &samplesReadperChannel);
        std::cout<<"The status from reading input samples is "<<status<<std::endl;
    }
    return 0;
}