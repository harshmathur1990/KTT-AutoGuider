//
// Created by IIAP-IPC on 03/02/2021.
//

#include "utilheaders.h"


int loggingMode;
std::string loggingFilename;
std::ofstream outfile;
char logString[1000];

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

int log(std::string logString, int severity) {
    if (severity == ERROR || loggingMode == DEBUG) {
        std::cout<<logString<<std::endl;
        outfile << logString << std::endl;
    }
    return 0;
}

int setupLogging(){
    time_t curr_time;
    tm * curr_tm;
    time(&curr_time);
    curr_tm = localtime(&curr_time);
    char filename[100];
    std::strftime(filename, 100, "output_%Y_%m_%dT%H_%M_%S.txt", curr_tm);
    loggingFilename = std::string(filename);
    outfile.open(loggingFilename.c_str());
    std::string loggingInputString;
    while (1) {
        std::cout<<"Enter the logging mode (0: error, 1: debug)"<<std::endl;
        getline(std::cin, loggingInputString);
        loggingMode = atoi(loggingInputString.c_str());
        if (loggingMode == ERROR || loggingMode == DEBUG) {
            std::string logMode = loggingMode == ERROR?std::string("ERROR"):std::string("DEBUG");
            sprintf(logString, "loggingMode: %s", logMode.c_str());
            log(logString, ERROR);
            break;
        }
    }
    return 0;
}