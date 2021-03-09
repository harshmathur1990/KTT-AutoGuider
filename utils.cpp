//
// Created by IIAP-IPC on 03/02/2021.
//

#include "utilheaders.h"

int getModeAndDeviceName(std::string deviceOptions[], std::string *deviceName, int *mode) {
    std::string modeString;
    while (1) {
        std::cout << "Enter 0 for Individual Mode, 1 for coupled mode" << std::endl;
        getline(std::cin, modeString);
        *mode = atoi(modeString.c_str());
        if (*mode == INDIVIDUAL || *mode == COUPLED) {
            switch (*mode) {
                case INDIVIDUAL:
                    *deviceName = deviceOptions[0];
                    break;
                case COUPLED:
                    *deviceName = deviceOptions[1];
                    break;
            }
            break;
        }
    }
    return 0;
}