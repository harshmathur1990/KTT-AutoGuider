#include <iostream>
#include "daqutils.cpp"


int main() {
    std::cout << "Hello, World!" << std::endl;
    initDAQ();
    setVoltages();
    return 0;
}