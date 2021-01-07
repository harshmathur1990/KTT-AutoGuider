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
    int reading;
    int i;
    char *token = NULL;
    while (1) {
        std::cout << "Enter e for exit or Number of sample Voltages to get with the reading: " << std::endl;
        getline (std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
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
        int32 samplesReadperChannel = 0;
        int status = getVoltage(readArray, &samplesReadperChannel, numOfVoltageSamples);
        if (status == 0) {
            std::cout<<"Number of samples per channel: " << samplesReadperChannel << std::endl;
            std::cout<<"Reading: " << reading << std::endl;
            for (int i=0; i <  samplesReadperChannel; i++) {
                std::cout << "Voltage[" << 0 << "] = " << readArray[i] * 1000 << " mVolts"<< std::endl;
            }
        }
        free(readArray);
    }
    return 0;
}