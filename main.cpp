#include <iostream>
#include <string>
#include <stdlib.h>
#include "motorcontrols.h"
#include "ra.h"
#include "dec.h"
#include "autoguider.h"

//double VoltageData[1];

int main() {
    std::string input;
    int mode;
    while (1) {
        std::cout << "Mode Select (Enter e for exit): " << std::endl;
        std::cout << "Enter 1 for RA test." << std::endl;
        std::cout << "Enter 2 for DEC test." << std::endl;
        std::cout << "Enter 3 for Motor test." << std::endl;
        std::cout << "Enter 4 for Serial loopback test." << std::endl;
        std::cout << "Enter 5 for RA close loop calibration." << std::endl;
        std::cout << "Enter 6 for DEC close loop calibration." << std::endl;
        std::cout << "Enter 7 for Monitoring RA voltage drift." << std::endl;
        std::cout << "Enter 8 for Monitoring DEC voltage drift." << std::endl;
        std::cout << "Enter 9 for Monitoring RA and DEC voltage drift." << std::endl;
        std::cout << "Enter 10 for Close loop operation." << std::endl;
        getline(std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            break;
        }
        if (input[0] == 'm' || input[0] == 'M') {
            continue;
        }
        mode = std::atoi(input.c_str());
        std::cout<<"Mode Selected:"<<mode<<std::endl;
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
                testSerialConnection();
                break;
            case 5:
                calibrateRA();
                break;
            case 6:
                calibrateDEC();
                break;
            case 7:
                monitorRA();
                break;
            case 8:
                monitorDEC();
                break;
            case 9:
                monitorRaAndDEC();
                break;
            case 10:
                closedLoop();
                break;
        }
    }
    return 0;
}