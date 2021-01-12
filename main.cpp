#include <iostream>
#include <string>
#include <stdlib.h>
#include "motorcontrols.h"
#include "ra.h"
#include "dec.h"

//double VoltageData[1];

int main() {
    std::string input;
    int mode;
    while (1) {
        std::cout << "Mode Select (Enter e for exit): " << std::endl;
        std::cout << "Enter 1 for RA test." << std::endl;
        std::cout << "Enter 2 for DEC test." << std::endl;
        std::cout << "Enter 3 for Motor test." << std::endl;
        std::cout << "Enter 4 for Close loop operation" << std::endl;
        getline(std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            break;
        }
        if (input[0] == 'm' || input[0] == 'M') {
            continue;
        }
        mode = std::atoi(input.c_str());
        switch (mode) {
            case 1:
                testRa();
                break;
            case 2:
                testDec();
                break;
            case 3:
                testMotor();
                break;
            case 4:
                closedLoop();
                break;
            default:
                break;
        }
    }
    return 0;
}