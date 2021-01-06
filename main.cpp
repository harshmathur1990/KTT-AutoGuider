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
        float64 *readArray = NULL;
        readArray = (float64*)calloc( numOfVoltageSamples, sizeof(float64));
        int32 samplesReadperChannel = 0;
        int status = getVoltage(readArray, &samplesReadperChannel, numOfVoltageSamples);
        if (status == 0) {
            std::cout<<"The number of samples per channel read are " << samplesReadperChannel << std::endl;
            for (int i=0; i <  samplesReadperChannel; i++) {
                std::cout << "Voltage[" << 0 << "] = " << readArray[i] * 1000 << " mVolts"<< std::endl;
            }
        }
        free(readArray);
    }
    return 0;
}