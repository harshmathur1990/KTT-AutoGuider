#include <iostream>
#include "daqutils.h"

double VoltageData[1];
TaskHandle DAQHandle;


int main() {
    std::cout << "Hello, World!" << std::endl;
    initDAQ();
    setVoltages();
    return 0;
}