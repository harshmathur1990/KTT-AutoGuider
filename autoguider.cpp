//
// Created by IIAP on 1/12/2021.
//

#include "autoguider.h"

bool pauseLoop;

int getCorrection(worker_params workerParams, float64 currMeanRA, float64 currMeanDEC, float64 refCountRA, float64 refCountDEC, float64 *prevErrorVoltageRA, float64 *integralErrorVoltageRA, float64 *curErrorVoltageRA, float64 *deltaErrorVoltageRA, float64 *prevErrorVoltageDEC, float64 *integralErrorVoltageDEC, float64 *curErrorVoltageDEC, float64 *deltaErrorVoltageDEC, int64 *raCorrection, int64 *decCorrection) {
    float64 countRA, countDEC, correctionVoltageRA, correctionVoltageDEC;
    if (workerParams.mode == INDIVIDUAL) {
        countRA = (currMeanRA - workerParams.raConstant) / workerParams.raSlope;
        std::cout << "Count RA: " << countRA << std::endl;
        *raCorrection = countRA - refCountRA;
        std::cout << "Correction RA:" << *raCorrection << std::endl;
        countDEC = (currMeanDEC - workerParams.decConstant) / workerParams.decSlope;
        std::cout << "Count DEC: " << countDEC << std::endl;
        *decCorrection = countDEC - refCountDEC;
        std::cout << "Correction DEC:" << *decCorrection << std::endl;
    }
    else {
        *curErrorVoltageRA = workerParams.raReferenceVoltage - currMeanRA;
        *curErrorVoltageDEC = workerParams.decReferenceVoltage - currMeanDEC;
        *integralErrorVoltageRA += *curErrorVoltageRA;
        *integralErrorVoltageDEC += *curErrorVoltageDEC;
        *deltaErrorVoltageRA = *curErrorVoltageRA - *prevErrorVoltageRA;
        *deltaErrorVoltageDEC = *curErrorVoltageDEC - *prevErrorVoltageDEC;
        std::cout<<"curErrorVoltageRA: "<<*curErrorVoltageRA<<std::endl;
        std::cout<<"curErrorVoltageDEC: "<<*curErrorVoltageDEC<<std::endl;
        std::cout<<"integralErrorVoltageRA: "<<*integralErrorVoltageRA<<std::endl;
        std::cout<<"integralErrorVoltageDEC: "<<*integralErrorVoltageDEC<<std::endl;
        std::cout<<"deltaErrorVoltageRA: "<<*deltaErrorVoltageRA<<std::endl;
        std::cout<<"deltaErrorVoltageDEC: "<<*deltaErrorVoltageDEC<<std::endl;
        correctionVoltageRA = workerParams.kp * *curErrorVoltageRA + workerParams.ki * *integralErrorVoltageRA + workerParams.kd * *deltaErrorVoltageRA;
        correctionVoltageDEC = workerParams.kp * *curErrorVoltageDEC + workerParams.ki * *integralErrorVoltageDEC + workerParams.kd * *deltaErrorVoltageDEC;
        *raCorrection = correctionVoltageRA * workerParams.raSlope + correctionVoltageDEC * workerParams.raConstant;
        *decCorrection = correctionVoltageRA * workerParams.decSlope + correctionVoltageDEC * workerParams.decConstant;
        std::cout<<"correctionVoltageRA: "<<correctionVoltageRA<<" correctionVoltageDEC: "<<correctionVoltageDEC<<std::endl;
        std::cout << "Correction RA:" << *raCorrection << std::endl;
        std::cout << "Correction DEC:" << *decCorrection << std::endl;
    }
    return 0;
}


DWORD WINAPI closedLoopThread(LPVOID lparam) {
    std::string input;
    worker_params &workerParams = *(worker_params*)lparam;
    float64 *readArray=NULL, *meanRA=NULL, *meanDEC=NULL, refCountRA, refCountDEC, countRA, countDEC, timeToCompleteMovement=0, prevMeanRA=0, currMeanRA=0, prevMeanDEC=0, currMeanDEC=0, quitCountRA=0, quitCountDEC=0, prevErrorVoltageRA=0, integralErrorVoltageRA=0, curErrorVoltageRA=0, deltaErrorVoltageRA=0, prevErrorVoltageDEC=0, integralErrorVoltageDEC=0, curErrorVoltageDEC=0, deltaErrorVoltageDEC=0;
    int64 decCorrection, raCorrection, RA=0, DEC=0, raDirection = 1, decDirection = 1, prevRaDirection = -1, prevDecDirection = -1, statusSetMotorCount=0, clock, readArraySize;
    const int64 raMotorNum = 2, decMotorNum = 1;
    int32 samplesReadPerChannel=0;
    refCountRA = (workerParams.raReferenceVoltage - workerParams.raConstant) / workerParams.raSlope;
    refCountDEC = (workerParams.decReferenceVoltage - workerParams.decConstant) / workerParams.decSlope;
    std::cout<<"Loop Update time (seconds):"<<workerParams.loopUpdateTimeInSeconds<<std::endl;
    std::cout<<"Polling time (miliseconds):"<<workerParams.pollingTimeInMiliSeconds<<std::endl;
    std::cout << "ReferenceVoltage RA: " << workerParams.raReferenceVoltage << std::endl;
//    std::cout << "refCount RA: " << refCountRA << std::endl;
    std::cout << "ReferenceVoltage DEC: " << workerParams.decReferenceVoltage << std::endl;
//    std::cout << "refCount DEC: " << refCountDEC << std::endl;
    const int64 counter = workerParams.loopUpdateTimeInSeconds * 1000 / workerParams.pollingTimeInMiliSeconds;
    meanRA = (float64*) calloc(counter, sizeof(float64));
    meanDEC = (float64*) calloc(counter, sizeof(float64));
    clock = 0;
    readArraySize = workerParams.numberOfVoltageSamples * 2;
    while (1) {
        if (quitCountRA == 3 || quitCountDEC == 3) {
            pauseLoop = true;
            std::cout<<"Image out of Sensor, Please readjust and press c"<<std::endl;
            while(pauseLoop == true) {
//                Sleep(1000);
            }
            clock = 0;
            quitCountRA = 0;
            quitCountDEC = 0;
            prevRaDirection = -1;
            prevDecDirection = -1;
        }
        meanRA[clock] = 0;
        meanDEC[clock] = 0;
        readArray = (float64*)calloc( readArraySize, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, workerParams.numberOfVoltageSamples, readArraySize);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            free(readArray);
            workerParams.statusWorker = -1;
            return -1;
        }
        getMeanRAAndDEC(readArray, readArraySize, samplesReadPerChannel, &meanRA[clock], &meanDEC[clock]);
        if (clock == counter) {
            prevMeanRA = currMeanRA;
            prevMeanDEC = currMeanDEC;
            currMeanRA = 0;
            currMeanDEC = 0;
            for (int j=0; j<counter;j++) {
                currMeanRA += meanRA[j];
                currMeanDEC += meanDEC[j];
            }
            currMeanRA /= counter;
            currMeanDEC /= counter;
            std::cout << "Mean Voltage RA: " << currMeanRA << std::endl;
            std::cout << "Mean Voltage DEC: " << currMeanDEC << std::endl;
            getCorrection(workerParams, currMeanRA, currMeanDEC, refCountRA, refCountDEC, &prevErrorVoltageRA, &integralErrorVoltageRA, &curErrorVoltageRA, &deltaErrorVoltageRA, &prevErrorVoltageDEC, &integralErrorVoltageDEC, &curErrorVoltageDEC, &deltaErrorVoltageDEC, &raCorrection, &decCorrection);
            RA = abs(raCorrection);
            DEC = abs(decCorrection);
            if (workerParams.mode == INDIVIDUAL) {
                raDirection = sgn(raCorrection) * sgn(workerParams.raReferenceVoltage);
                decDirection = sgn(decCorrection) * sgn(workerParams.decReferenceVoltage) * -1;
            }
            else {
                raDirection = sgn(raCorrection);
                decDirection = sgn(decCorrection);
            }

            if (RA > abs(workerParams.maxVoltageChangeInMiliVoltsPerSecRA)) {
//                RA = abs(workerParams.maxVoltageChangeInMiliVoltsPerSecRA);
                if(prevRaDirection == raDirection)
                    quitCountRA += 1;
            }
            else {
                quitCountRA = 0;
            }
            if (DEC > abs(workerParams.maxVoltageChangeInMiliVoltsPerSecDEC)) {
//                DEC = abs(workerParams.maxVoltageChangeInMiliVoltsPerSecDEC);
                if (prevDecDirection == decDirection)
                    quitCountDEC += 1;
            }
            else {
                quitCountDEC = 0;
            }
            prevRaDirection = raDirection;
            prevDecDirection = decDirection;
            if (quitCountRA == 3 || quitCountDEC == 3)
                continue;

            std::cout << "Applying counts RA:" << raDirection * RA << std::endl;
            statusSetMotorCount = setMotorCount(raMotorNum, raDirection, RA);
            if (statusSetMotorCount != 0) {
                free(readArray);
                workerParams.statusWorker = -2;
                return -2;
            }

            std::cout << "Applying counts DEC:" << decDirection * DEC << std::endl;
            statusSetMotorCount = setMotorCount(decMotorNum, decDirection, DEC);

            if (statusSetMotorCount != 0) {
                free(readArray);
                workerParams.statusWorker = -2;
                return -2;
            }
            clock = -1;
        }
        clock += 1;
        free(readArray);
        Sleep(workerParams.pollingTimeInMiliSeconds);
    }
}

int getMeanRAAndDEC(float64 *readArray, int64 readArraySize, int32 samplesReadPerChannel, float64 *meanRA,
                    float64 *meanDEC) {
    *meanRA = 0;
    *meanDEC = 0;
    for (int i=0; i <  samplesReadPerChannel * 2; i+=2) {
        *meanRA += readArray[i+1] * 1000;
        *meanDEC += readArray[i] * 1000;
    }
    *meanRA /= samplesReadPerChannel;
    *meanDEC /= samplesReadPerChannel;
    return 0;
}

int closedLoop(){
    std::string input, slopeString, constantString, referenceVoltageString, numberOfVoltageSamplesString, frequencyString, loopUpdateTimeInSecondsString, pollingTimeInMiliSecondsString, maxVoltageChangeInMiliVoltsPerSecString;
    float64 raSlope, decSlope, raConstant, decConstant, raReferenceVoltage, decReferenceVoltage, maxVoltageChangeInMiliVoltsPerSecRA, maxVoltageChangeInMiliVoltsPerSecDEC, loopUpdateTimeInSeconds, pollingTimeInMiliSeconds;
    int64 numberOfVoltageSamples, raFrequency, decFrequency, comPortNum, raMotorNum=2, decMotorNum = 1;
    std::string deviceNameOptions[] = {"Dev1/ai1:2", "Dev1/ai1:2"};
    std::string deviceName;
    int mode;
    getModeAndDeviceName(deviceNameOptions, &deviceName, &mode);
    std::cout<<"Sample Input Mode: "<<mode<<std::endl;
    std::cout<<"Device Name: "<<deviceName<<std::endl;
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
    int statusInitDaQChain = initDAQAIChan(deviceName.c_str());
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
    loopUpdateTimeInSeconds = atof(loopUpdateTimeInSecondsString.c_str());
    std::cout<<"Enter the polling time in miliseconds:"<<std::endl;
    getline(std::cin, pollingTimeInMiliSecondsString);
    pollingTimeInMiliSeconds = atof(pollingTimeInMiliSecondsString.c_str());
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
    workerParams.pollingTimeInMiliSeconds = pollingTimeInMiliSeconds;
    workerParams.maxVoltageChangeInMiliVoltsPerSecRA = maxVoltageChangeInMiliVoltsPerSecRA;
    workerParams.maxVoltageChangeInMiliVoltsPerSecDEC = maxVoltageChangeInMiliVoltsPerSecDEC;
    workerParams.mode = mode;
    workerParams.statusWorker = 0;
    if (mode == INDIVIDUAL) {
        workerParams.kp = 1;
        workerParams.ki = 0;
        workerParams.kd = 0;
    }
    else {
        std::cout<<"Enter 1 to give your own kp, ki, kd."<<std::endl;
        getline(std::cin, input);
        int choice = atoi(input.c_str());
        if (choice == 1) {
            std::cout<<"Enter the value of Kp:"<<std::endl;
            getline(std::cin, input);
            workerParams.kp = atof(input.c_str());
            std::cout<<"Enter the value of Ki:"<<std::endl;
            getline(std::cin, input);
            workerParams.ki = atof(input.c_str());
            std::cout<<"Enter the value of Kd:"<<std::endl;
            getline(std::cin, input);
            workerParams.kd = atof(input.c_str());
        }
        else{
            workerParams.kp = 1.015;
            workerParams.ki = 0.004;
            workerParams.kd = 0.11;
        }
    }
    std::cout<<"Kp: "<<workerParams.kp<<" Ki: "<<workerParams.ki<<" Kd: "<<workerParams.kd<<std::endl;
    bool inputAccepted;
    float64 meanRA, meanDEC, *readArray=NULL;
    int32 samplesReadPerChannel=0;
    int64 readArraySize;
    readArraySize = numberOfVoltageSamples * 2;
    while(1) {
        inputAccepted=false;
        meanRA = 0;
        meanDEC = 0;
        readArray = (float64*)calloc( readArraySize, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, workerParams.numberOfVoltageSamples, readArraySize);
        if (statusReadVoltage != 0) {
            std::cout<<"Failed to get Voltage"<<std::endl;
            free(readArray);
            workerParams.statusWorker = -1;
            return -1;
        }
        getMeanRAAndDEC(readArray, readArraySize, samplesReadPerChannel, &meanRA, &meanDEC);
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
    pauseLoop = false;
    myHandle = CreateThread(0, 0, closedLoopThread, &workerParams, 0, &myThreadID);
    std::cout<<"Press q to quit close loop operation."<<std::endl;
    while(slopeString[0] != 'q' ) {
        if (pauseLoop == true && (slopeString[0] == 'c' || slopeString[0]== 'C')) {
            pauseLoop = false;
        }
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

int calibrateChannel(int motorNum, const char deviceName[], int mode) {
    std::cout<<"Samples Input Mode: "<<mode<<std::endl;
    int comPortNum, frequency, count, step, direction, numOfVoltageSamples;
    int64 readArraySize;
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
    if (mode == INDIVIDUAL)
        readArraySize = numOfVoltageSamples;
    else
        readArraySize = numOfVoltageSamples * 2;
    while(totalSteps < count) {
        readArray = (float64*)calloc( readArraySize, sizeof(float64));
        int statusReadVoltage = getVoltage(readArray, &samplesReadPerChannel, numOfVoltageSamples, readArraySize);
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
        for (int i=0; i <  readArraySize; i++) {
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
    std::string deviceName;
    std::string deviceNameOptions[] = {"Dev1/ai2", "Dev1/ai2:1"};
    std::string modeString;
    int mode;
    getModeAndDeviceName(deviceNameOptions, &deviceName, &mode);
    return calibrateChannel(motorNum, deviceName.c_str(), mode);
}

int calibrateDEC() {
    int motorNum = 1;
    std::string deviceName;
    std::string deviceNameOptions[] = {"Dev1/ai1", "Dev1/ai1:2"};
    std::string modeString;
    int mode;
    getModeAndDeviceName(deviceNameOptions, &deviceName, &mode);
    return calibrateChannel(motorNum, deviceName.c_str(), mode);
}

int monitorRaAndDEC() {
    std::string deviceName = "Dev1/ai1:2";
    int numOfChannels = 2;
    return monitorChannel(deviceName.c_str(), numOfChannels);
}