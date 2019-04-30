#include <iostream>
#include <string>
#include <stdlib.h>
#include "daqutils.h"

double VoltageData[1];
TaskHandle DAQHandle;


int main() {
    std::cout << "Hello, World!" << std::endl;
    initDAQ();
    std::string input;
    double voltage;
    while (1) {
        std::cout << "Enter e for exit or Voltage to Set: " << std::endl;
        getline (std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            break;
        }
        voltage = atof(input.c_str());
        std::cout<<"The value of voltage to be applied is: "<< voltage<<std::endl;
        setVoltages(voltage);
    }
    return 0;
}