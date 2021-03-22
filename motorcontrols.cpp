//
// Created by harsh on 1/12/21.
//

#include "motorcontrols.h"

int CreateControllerConnection(int ComPortNum) {
    int status = createSerialConnection(
            ComPortNum,
            115200,
            8,
            ONESTOPBIT,
            NOPARITY);
    return status;
}

int closeControllerConnection() {
    int status = closeSerialConnection();
    return status;
}

int setMotorFrequency(int motorNum, int frequency) {
    std::string writeWord = std::to_string(motorNum).append("F").append(std::to_string(frequency));
    int status = writeToPort(writeWord);
    return status;
}

int setMotorCount(int motorNum, int direction, int counts) {
    std::string directionString = direction == 1?"+":"-";
    std::string writeWord = std::to_string(motorNum).append(directionString).append(std::to_string(counts));
    int status = writeToPort(writeWord);
    return status;
}

int enableMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append(std::string("E"));
    int status = writeToPort(writeWord);
    return status;
}

int disableMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append(std::string("D"));
    int status = writeToPort(writeWord);
    return status;
}

int exitMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append("Q");
    int status = writeToPort(writeWord);
    return status;
}

int testMotor() {
    std::string input;
    std::sprintf(logString, "Enter the COM Port number:");
    log(logString, PROMPT);
    getline(std::cin, input);
    int comPortNum = std::atoi(input.c_str());
    int status = CreateControllerConnection(comPortNum);
    std::string quitString;
    if (status == 0) {
        while (1) {
            std::string motorNumString;
            std::string frequencyString;
            std::string countString;
            std::string directionString;
            std::sprintf(logString, "Enter e to exit, m for mode switch or enter the motor number:");
            log(logString, PROMPT);
            getline(std::cin, motorNumString);
            if (motorNumString[0] == 'e' || motorNumString[0] == 'E') {
                closeControllerConnection();
                exit(0);
            }
            if (motorNumString[0] == 'm' || motorNumString[0] == 'M') {
                break;
            }
            int motorNum = std::atoi(motorNumString.c_str());
            enableMotor(motorNum);
            std::sprintf(logString, "Enter the Motor Frequency: ");
            log(logString, PROMPT);
            getline(std::cin, frequencyString);
            int frequency = std::atoi((frequencyString.c_str()));
            int statusMotorFrequency = setMotorFrequency(motorNum, frequency);
            if (statusMotorFrequency == 0) {
                std::sprintf(logString, "Enter the Motor Direction: ");
                log(logString, PROMPT);
                getline(std::cin, directionString);
                std::sprintf(logString, "Enter the Motor Counts to move: ");
                log(logString, PROMPT);
                getline(std::cin, countString);
                int direction = std::atoi(directionString.c_str());
                int counts = std::atoi(countString.c_str());
                setMotorCount(motorNum, direction, counts);
                std::sprintf(logString, "Press Enter to quit the motor.");
                log(logString, PROMPT);
                getline(std::cin, motorNumString);
                exitMotor(motorNum);
                disableMotor(motorNum);
            }
        }
        closeControllerConnection();
    }
    return 0;
}