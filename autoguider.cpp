//
// Created by IIAP on 1/12/2021.
//

#include "autoguider.h"

bool runClosedLoop;

int closedLoopWorker() {

}

int closedLoop(){
    std::string slopeString, constantString, referenceVoltageString, numberOfVoltageSamplesString;
    float64 slope, constant, referenceVoltage;
    int64 numberOfVoltageSamples;
    std::cout<<"Enter the slope of the calibration line equation:"<<std::endl;
    getline(std::cin, slopeString);
    std::cout<<"Enter the constant of the calibration line equation:"<<std::endl;
    getline(std::cin, constantString);
    std::cout<<"Enter the reference voltage:"<<std::endl;
    getline(std::cin, referenceVoltageString);
    slope = atof(slopeString.c_str());
    constant = atof(constantString.c_str());
    referenceVoltage = atof(referenceVoltageString.c_str());
    std::cout<<"Enter the number of voltage samples:"<<std::endl;
    getline(std::cin, numberOfVoltageSamplesString);
    numberOfVoltageSamples = atoi(numberOfVoltageSamplesString.c_str());
    return 0;
}

int calibrateChannel(int motorNum, const char deviceName[]) {
    int comPortNum, frequency, count, step, direction, numOfVoltageSamples;
    int32 samplesReadPerChannel = 0;
    std::string input, numOfVoltageSamplesString;
    std::cout<<"Enter the COM Port number: "<<std::endl;
    getline(std::cin, input);
    comPortNum = std::atoi(input.c_str());
    int status = CreateControllerConnection(comPortNum);
    if (status != 0) {
        std::cout<<"Unable to connect to motor"<<std::endl;
        return -1;
    }
    std::string frequencyString;
    std::string countString;
    std::string stepsString;
    std::string directionString;
    std::cout<<"Enter the frequency of motor: "<<std::endl;
    getline(std::cin, frequencyString);
    std::cout<<"Enter the total counts to move: "<<std::endl;
    getline(std::cin, countString);
    std::cout<<"Enter the step size in counts: "<<std::endl;
    getline(std::cin, stepsString);
    std::cout<<"Enter the motor direction: "<<std::endl;
    getline(std::cin, directionString);
    frequency = std::atoi(frequencyString.c_str());
    count = std::atoi(countString.c_str());
    step = std::atoi(stepsString.c_str());
    direction = std::atoi(directionString.c_str());
    int statusMotorEnable = enableMotor(motorNum);
    if (statusMotorEnable != 0) {
        std::cout<<"Failed to Enable Motor"<<std::endl;
        closeControllerConnection();
        return -2;
    }
    int statusMotorFrequency = setMotorFrequency(motorNum, frequency);
    if (statusMotorFrequency != 0) {
        std::cout<<"Failed to Set Motor Frequency"<<std::endl;
        disableMotor(motorNum);
        closeControllerConnection();
        return -3;
    }
    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        disableMotor(motorNum);
        closeControllerConnection();
        return -4;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName);
    if (statusInitDaQChain != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        disableMotor(motorNum);
        closeControllerConnection();
        clearDAQTask();
        return -5;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::cout<<"Failed to Start DaQ Task"<<std::endl;
        disableMotor(motorNum);
        closeControllerConnection();
        clearDAQTask();
        return -6;
    }
    std::cout<<"Enter number of Voltage Samples to Get:"<<std::endl;
    getline(std::cin, numOfVoltageSamplesString);
    numOfVoltageSamples = std::atoi(numOfVoltageSamplesString.c_str());
    float64 *readArray = NULL;
    float64 timeToCompleteMovement = step * 1000/frequency;
    float64 goBackTime = count * 1000/frequency;
    float loopUpdateTime = 100;
    int totalSteps = 0;
    int statusSetMotorCount = 0;
    while(totalSteps < count) {
        readArray = (float64*)calloc( numOfVoltageSamples, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, numOfVoltageSamples, numOfVoltageSamples);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            exitMotor(motorNum);
            disableMotor(motorNum);
            closeControllerConnection();
            stopDAQTask();
            clearDAQTask();
            free(readArray);
            return -7;
        }
        std::cout<<totalSteps;
        for (int i=0; i <  samplesReadPerChannel; i++) {

            std::cout << "  " << readArray[i] * 1000;
        }
        std::cout<<std::endl;
        statusSetMotorCount = setMotorCount(motorNum, direction, step);
        if (statusSetMotorCount != 0) {
            std::cout<<"Failed to move Motor"<<std::endl;
            exitMotor(motorNum);
            disableMotor(motorNum);
            closeControllerConnection();
            stopDAQTask();
            clearDAQTask();
            free(readArray);
            return -8;
        }
        Sleep(timeToCompleteMovement * 3);
//        Sleep(loopUpdateTime);
        totalSteps += step;
        free(readArray);
    }
    statusSetMotorCount = setMotorCount(motorNum, -direction, totalSteps);
    if (statusSetMotorCount != 0) {
        std::cout<<"Failed to move Motor back to initial position"<<std::endl;
        exitMotor(motorNum);
        disableMotor(motorNum);
        closeControllerConnection();
        stopDAQTask();
        clearDAQTask();
        return -9;
    }
    Sleep(goBackTime * 2);
    std::cout<<"Test Successfully Completed"<<std::endl;
    exitMotor(motorNum);
    disableMotor(motorNum);
    closeControllerConnection();
    stopDAQTask();
    clearDAQTask();
    return 0;
}

int calibrateRA() {
    int motorNum = 2;
    const char deviceName[] = "Dev1/ai2";
    return calibrateChannel(motorNum, deviceName);
}

int calibrateDEC() {
    int motorNum = 1;
    const char deviceName[] = "Dev1/ai1";
    return calibrateChannel(motorNum, deviceName);
}