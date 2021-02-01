//
// Created by IIAP on 1/12/2021.
//

#include "autoguider.h"

bool runClosedLoop;

DWORD WINAPI closedLoopThread(LPVOID lparam) {
    std::string input;
    worker_params &workerParams = *(worker_params*)lparam;
    float64 *readArray=NULL, meanRA, meanDEC, refCountRA, refCountDEC, countRA, countDEC, timeToCompleteMovement=0;
    int64 decCorrrection, raCorrection, RA=0, DEC=0, direction = 1, statusSetMotorCount=0;
    const int64 raMotorNum = 2, decMotorNum = 1;
    int32 samplesReadPerChannel=0;
    refCountRA = (workerParams.raReferenceVoltage - workerParams.raConstant) / workerParams.raSlope;
    refCountDEC = (workerParams.decReferenceVoltage - workerParams.decConstant) / workerParams.decSlope;
    std::cout<<"Loop Update time (seconds):"<<workerParams.loopUpdateTimeInSeconds<<std::endl;
    std::cout << "ReferenceVoltage RA: " << workerParams.raReferenceVoltage << std::endl;
    std::cout << "refCount RA: " << refCountRA << std::endl;
    std::cout << "ReferenceVoltage DEC: " << workerParams.decReferenceVoltage << std::endl;
    std::cout << "refCount DEC: " << refCountDEC << std::endl;
    while (1) {
        Sleep(workerParams.loopUpdateTimeInSeconds * 1000);
        meanRA = 0;
        meanDEC = 0;
        readArray = (float64*)calloc( workerParams.numberOfVoltageSamples * 2, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, workerParams.numberOfVoltageSamples, workerParams.numberOfVoltageSamples * 2);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            free(readArray);
            workerParams.statusWorker = -1;
            return -1;
        }
        for (int i=0; i <  samplesReadPerChannel * 2; i+=2) {
            meanRA += readArray[i+1] * 1000;
            meanDEC += readArray[i] * 1000;
        }
        meanRA /= samplesReadPerChannel;
        meanDEC /= samplesReadPerChannel;
        std::cout << "Mean Voltage RA: " << meanRA << std::endl;
        std::cout << "Mean Voltage DEC: " << meanDEC << std::endl;
        countRA = (meanRA - workerParams.raConstant) / workerParams.raSlope;
        std::cout << "Count RA: " << countRA << std::endl;
        raCorrection = countRA - refCountRA;
        std::cout << "Correction RA:" << raCorrection << std::endl;
        countDEC = (meanDEC - workerParams.decConstant) / workerParams.decSlope;
        std::cout << "Count DEC: " << countDEC << std::endl;
        decCorrrection = countDEC - refCountDEC;
        std::cout << "Correction DEC:" << decCorrrection << std::endl;
//        if (abs(raCorrection) > maxCorrectionPerSec * loopUpdateTimeInSeconds) {
//            RA = maxCorrectionPerSec * loopUpdateTimeInSeconds;
//        }
//        else {
//            RA = abs(raCorrection);
//        }
        RA = abs(raCorrection);
        DEC = abs(decCorrrection);
        std::cout << "Applying counts RA:" << RA << std::endl;
        std::cout << "Applying counts DEC:" << DEC << std::endl;
        direction = sgn(raCorrection) * sgn(workerParams.raReferenceVoltage);
        statusSetMotorCount = setMotorCount(raMotorNum, direction, RA);
//        timeToCompleteMovement = RA / workerParams.raFrequency;
        if (statusSetMotorCount != 0) {
            free(readArray);
            workerParams.statusWorker = -2;
            return -2;
        }
        direction = sgn(decCorrrection) * sgn(workerParams.decReferenceVoltage);
        statusSetMotorCount = setMotorCount(decMotorNum, direction, DEC);
//        timeToCompleteMovement = RA / workerParams.raFrequency;
        if (statusSetMotorCount != 0) {
            free(readArray);
            workerParams.statusWorker = -2;
            return -2;
        }
        free(readArray);
    }
}

int closedLoop(){
    std::string input, slopeString, constantString, referenceVoltageString, numberOfVoltageSamplesString, frequencyString, loopUpdateTimeInSecondsString, maxVoltageChangeInMiliVoltsPerSecString;
    float64 raSlope, decSlope, raConstant, decConstant, raReferenceVoltage, decReferenceVoltage, maxVoltageChangeInMiliVoltsPerSecRA, maxVoltageChangeInMiliVoltsPerSecDEC;
    int64 numberOfVoltageSamples, raFrequency, decFrequency, comPortNum, raMotorNum=2, decMotorNum = 1, loopUpdateTimeInSeconds;
    const char deviceName[] = "Dev1/ai1:2";
    std::cout<<"Enter the Slope of the RA calibration line equation:"<<std::endl;
    getline(std::cin, slopeString);
    std::cout<<"Enter the Constant of the RA calibration line equation:"<<std::endl;
    getline(std::cin, constantString);
    std::cout<<"Enter the RA reference voltage:"<<std::endl;
    getline(std::cin, referenceVoltageString);
    raSlope = atof(slopeString.c_str());
    raConstant = atof(constantString.c_str());
    raReferenceVoltage = atof(referenceVoltageString.c_str());
    std::cout<<"Enter the Slope of the DEC calibration line equation:"<<std::endl;
    getline(std::cin, slopeString);
    std::cout<<"Enter the Constant of the DEC calibration line equation:"<<std::endl;
    getline(std::cin, constantString);
    std::cout<<"Enter the DEC reference voltage:"<<std::endl;
    getline(std::cin, referenceVoltageString);
    decSlope = atof(slopeString.c_str());
    decConstant = atof(constantString.c_str());
    decReferenceVoltage = atof(referenceVoltageString.c_str());
    std::cout<<"Enter the COM Port number: "<<std::endl;
    getline(std::cin, input);
    comPortNum = std::atoi(input.c_str());
    int status = CreateControllerConnection(comPortNum);
    if (status != 0) {
        std::cout<<"Unable to connect to motor"<<std::endl;
        return -1;
    }
    int statusMotorEnable = enableMotor(raMotorNum);
    if (statusMotorEnable != 0) {
        std::cout<<"Failed to Enable RA Motor"<<std::endl;
        closeControllerConnection();
        return -2;
    }
    statusMotorEnable = enableMotor(decMotorNum);
    if (statusMotorEnable != 0) {
        std::cout<<"Failed to Enable DEC Motor"<<std::endl;
        disableMotor(raMotorNum);
        closeControllerConnection();
        return -2;
    }
    std::cout<<"Enter the Frequency of RA motor:"<<std::endl;
    getline(std::cin, frequencyString);
    raFrequency = atoi(frequencyString.c_str());
    std::cout<<"Enter the Frequency of DEC motor:"<<std::endl;
    getline(std::cin, frequencyString);
    decFrequency = atoi(frequencyString.c_str());
    int statusMotorFrequency = setMotorFrequency(raMotorNum, raFrequency);
    if (statusMotorFrequency != 0) {
        std::cout<<"Failed to Set RA Motor Frequency"<<std::endl;
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -3;
    }
    statusMotorFrequency = setMotorFrequency(decMotorNum, decFrequency);
    if (statusMotorFrequency != 0) {
        std::cout<<"Failed to Set DEC Motor Frequency"<<std::endl;
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -3;
    }
    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -4;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName);
    if (statusInitDaQChain != 0) {
        std::cout<<"Failed to Create DaQ Task"<<std::endl;
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        clearDAQTask();
        return -5;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::cout<<"Failed to Start DaQ Task"<<std::endl;
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        clearDAQTask();
        return -6;
    }
    std::cout<<"Enter the number of voltage samples:"<<std::endl;
    getline(std::cin, numberOfVoltageSamplesString);
    numberOfVoltageSamples = atoi(numberOfVoltageSamplesString.c_str());
    std::cout<<"Enter the loop update time in seconds:"<<std::endl;
    getline(std::cin, loopUpdateTimeInSecondsString);
    loopUpdateTimeInSeconds = atoi(loopUpdateTimeInSecondsString.c_str());
    std::cout<<"Enter the max permissible voltage change per second in RA:"<<std::endl;
    getline(std::cin, maxVoltageChangeInMiliVoltsPerSecString);
    maxVoltageChangeInMiliVoltsPerSecRA = atoi(maxVoltageChangeInMiliVoltsPerSecString.c_str());
    std::cout<<"Enter the max permissible voltage change per second in DEC:"<<std::endl;
    getline(std::cin, maxVoltageChangeInMiliVoltsPerSecString);
    maxVoltageChangeInMiliVoltsPerSecDEC = atoi(maxVoltageChangeInMiliVoltsPerSecString.c_str());
    DWORD myThreadID;
    int64 statusWorker;
    HANDLE myHandle;
    worker_params workerParams;
    workerParams.raSlope = raSlope;
    workerParams.raConstant = raConstant;
    workerParams.raReferenceVoltage = raReferenceVoltage;
    workerParams.decSlope = decSlope;
    workerParams.decConstant = decConstant;
    workerParams.decReferenceVoltage = decReferenceVoltage;
    workerParams.numberOfVoltageSamples = numberOfVoltageSamples;
    workerParams.raFrequency = raFrequency;
    workerParams.decFrequency = decFrequency;
    workerParams.loopUpdateTimeInSeconds = loopUpdateTimeInSeconds;
    workerParams.maxVoltageChangeInMiliVoltsPerSecRA = maxVoltageChangeInMiliVoltsPerSecRA;
    workerParams.maxVoltageChangeInMiliVoltsPerSecDEC = maxVoltageChangeInMiliVoltsPerSecDEC;
    workerParams.statusWorker = 0;
    bool inputAccepted;
    float64 meanRA, meanDEC, *readArray=NULL;
    int32 samplesReadPerChannel=0;
    while(1) {
        inputAccepted=false;
        meanRA = 0;
        meanDEC = 0;
        readArray = (float64*)calloc( workerParams.numberOfVoltageSamples * 2, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, workerParams.numberOfVoltageSamples, workerParams.numberOfVoltageSamples * 2);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            free(readArray);
            workerParams.statusWorker = -1;
            return -1;
        }
        for (int i=0; i <  samplesReadPerChannel * 2; i+=2) {
            meanRA += readArray[i+1] * 1000;
            meanDEC += readArray[i] * 1000;
        }
        meanRA /= samplesReadPerChannel;
        meanDEC /= samplesReadPerChannel;
        std::cout << "Mean RA: " << meanRA << std::endl;
        std::cout << "Mean DEC: " << meanDEC << std::endl;
        while (1) {
            std::cout<<"Position accepted?y/n"<<std::endl;
            getline(std::cin, input);
            if (input[0] == 'Y' || input[0] == 'y') {
                inputAccepted = true;
                break;
            }
            if (input[0] == 'N' || input[0] == 'n') {
                inputAccepted = false;
                break;
            }
        }
        free(readArray);
        if (inputAccepted) break;
    }
    myHandle = CreateThread(0, 0, closedLoopThread, &workerParams, 0, &myThreadID);
    std::cout<<"Press q to quit close loop operation."<<std::endl;
    while(slopeString[0] != 'q' ) {
        getline(std::cin, slopeString);
    }
    CloseHandle(myHandle);
    std::cout<<"Finishing the Closed Loop Process"<<std::endl;
    exitMotor(raMotorNum);
    disableMotor(raMotorNum);
    exitMotor(decMotorNum);
    disableMotor(decMotorNum);
    closeControllerConnection();
    stopDAQTask();
    clearDAQTask();
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
    std::cout<<"Enter the raFrequency of motor: "<<std::endl;
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

int monitorRaAndDEC() {
    const char deviceName[] = "Dev1/ai1:2";
    return monitorChannel(deviceName, 2);
}