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
    std::string status_str = status == 0?"True":"False";
    std::cout << "Creating Controller Connection: " << status_str << std::endl;
    return status;
}

int closeControllerConnection() {
    int status = closeSerialConnection();
    std::string status_str = status == 0?"True":"False";
    std::cout << "Closing Controller Connection: " << status_str << std::endl;
    return status;
}

int setMotorFrequency(int motorNum, int frequency) {
    std::string writeWord = std::to_string(motorNum).append("F").append(std::to_string(frequency));
    int status = writeToPort(writeWord);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Wrote to Motor: " << status_str << std::endl;
    return status;
}

int setMotorCount(int motorNum, int direction, int counts) {
    std::string directionString = direction == 1?"+":"-";
    std::string writeWord = std::to_string(motorNum).append(directionString).append(std::to_string(counts));
    int status = writeToPort(writeWord);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Wrote to Motor: " << status_str << std::endl;
    return status;
}

int enableMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append(std::string("E"));
    int status = writeToPort(writeWord);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Wrote to Motor: " << status_str << std::endl;
    return status;
}

int disableMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append(std::string("D"));
    int status = writeToPort(writeWord);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Wrote to Motor: " << status_str << std::endl;
    return status;
}

int exitMotor(int motorNum) {
    std::string writeWord = std::to_string(motorNum).append("Q");
    int status = writeToPort(writeWord);
    std::string status_str = status == 0?"True":"False";
    std::cout << "Wrote to Motor: " << status_str << std::endl;
    return status;
}

int testMotor() {
    std::string input;
    std::cout<<"Enter the COM Port number: "<<std::endl;
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
            std::cout<<"Enter e to exit, m for mode switch or enter the motor number:" <<std::endl;
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
            std::cout<<"Enter the Motor Frequency: "<<std::endl;
            getline(std::cin, frequencyString);
            int frequency = std::atoi((frequencyString.c_str()));
            int statusMotorFrequency = setMotorFrequency(motorNum, frequency);
            if (statusMotorFrequency == 0) {
                std::cout << "Enter the Motor Direction: " << std::endl;
                getline(std::cin, directionString);
                std::cout << "Enter the Motor Counts to move: " << std::endl;
                getline(std::cin, countString);
                int direction = std::atoi(directionString.c_str());
                int counts = std::atoi(countString.c_str());
                setMotorCount(motorNum, direction, counts);
                std::cout<<"Press Enter to quit the motor"<<std::endl;
                getline(std::cin, motorNumString);
                exitMotor(motorNum);
                disableMotor(motorNum);
            }
        }
        closeControllerConnection();
    }
    return 0;
}