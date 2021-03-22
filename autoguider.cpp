//
// Created by IIAP on 1/12/2021.
//

#include "autoguider.h"

bool pauseLoop;

int getCorrection(worker_params workerParams, float64 currMeanRA, float64 currMeanDEC, float64 refCountRA, float64 refCountDEC, float64 *prevErrorVoltageRA, float64 *integralErrorVoltageRA, float64 *curErrorVoltageRA, float64 *deltaErrorVoltageRA, float64 *prevErrorVoltageDEC, float64 *integralErrorVoltageDEC, float64 *curErrorVoltageDEC, float64 *deltaErrorVoltageDEC, int64 *raCorrection, int64 *decCorrection) {
    float64 countRA, countDEC, correctionVoltageRA, correctionVoltageDEC;
    if (workerParams.mode == INDIVIDUAL) {
        countRA = (currMeanRA - workerParams.raConstant) / workerParams.raSlope;
        std::sprintf(logString, "Count RA: %.2f", countRA);
        log(logString, DEBUG);
        *raCorrection = countRA - refCountRA;
        std::sprintf(logString, "Correction RA: %d", *raCorrection);
        log(logString, DEBUG);
        countDEC = (currMeanDEC - workerParams.decConstant) / workerParams.decSlope;
        std::cout << "Count DEC: " << countDEC << std::endl;
        std::sprintf(logString, "Count DEC: %.2f", countDEC);
        log(logString, DEBUG);
        *decCorrection = countDEC - refCountDEC;
        std::sprintf(logString, "Correction DEC: %d", *decCorrection);
        log(logString, DEBUG);
    }
    else {
        *curErrorVoltageRA = workerParams.raReferenceVoltage - currMeanRA;
        *curErrorVoltageDEC = workerParams.decReferenceVoltage - currMeanDEC;
        *integralErrorVoltageRA += *curErrorVoltageRA;
        *integralErrorVoltageDEC += *curErrorVoltageDEC;
        *deltaErrorVoltageRA = *curErrorVoltageRA - *prevErrorVoltageRA;
        *deltaErrorVoltageDEC = *curErrorVoltageDEC - *prevErrorVoltageDEC;
        std::sprintf(logString, "curErrorVoltageRA:  %.2f", *curErrorVoltageRA);
        log(logString, DEBUG);
        std::sprintf(logString, "curErrorVoltageDEC:  %.2f", *curErrorVoltageDEC);
        log(logString, DEBUG);
        std::sprintf(logString, "integralErrorVoltageRA:  %.2f", *integralErrorVoltageRA);
        log(logString, DEBUG);
        std::sprintf(logString, "integralErrorVoltageDEC:  %.2f", *integralErrorVoltageDEC);
        log(logString, DEBUG);
        std::sprintf(logString, "deltaErrorVoltageRA:  %.2f", *deltaErrorVoltageRA);
        log(logString, DEBUG);
        std::sprintf(logString, "deltaErrorVoltageDEC:  %.2f", *deltaErrorVoltageDEC);
        log(logString, DEBUG);
        correctionVoltageRA = workerParams.kp * *curErrorVoltageRA + workerParams.ki * *integralErrorVoltageRA + workerParams.kd * *deltaErrorVoltageRA;
        correctionVoltageDEC = workerParams.kp * *curErrorVoltageDEC + workerParams.ki * *integralErrorVoltageDEC + workerParams.kd * *deltaErrorVoltageDEC;
        *raCorrection = correctionVoltageRA * workerParams.raSlope + correctionVoltageDEC * workerParams.raConstant;
        *decCorrection = correctionVoltageRA * workerParams.decSlope + correctionVoltageDEC * workerParams.decConstant;
        std::sprintf(logString, "correctionVoltageRA:  %.2f correctionVoltageDEC: %.2f", correctionVoltageRA, correctionVoltageDEC);
        log(logString, DEBUG);
        std::sprintf(logString, "Correction RA:  %.2f", *raCorrection);
        log(logString, DEBUG);
        std::sprintf(logString, "Correction DEC:  %.2f", *decCorrection);
        log(logString, DEBUG);
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
    std::sprintf(logString, "Loop Update time (seconds):  %.2f", workerParams.loopUpdateTimeInSeconds);
    log(logString, INFO);
    std::sprintf(logString, "Polling time (miliseconds):  %.2f", workerParams.pollingTimeInMiliSeconds);
    log(logString, INFO);
    std::sprintf(logString, "ReferenceVoltage RA:  %.2f", workerParams.raReferenceVoltage);
    log(logString, INFO);
    std::sprintf(logString, "ReferenceVoltage DEC:  %.2f", workerParams.decReferenceVoltage);
    log(logString, INFO);
    const int64 counter = workerParams.loopUpdateTimeInSeconds * 1000 / workerParams.pollingTimeInMiliSeconds;
    meanRA = (float64*) calloc(counter, sizeof(float64));
    meanDEC = (float64*) calloc(counter, sizeof(float64));
    clock = 0;
    readArraySize = workerParams.numberOfVoltageSamples * 2;
    while (1) {
        if (quitCountRA == 3 || quitCountDEC == 3) {
            pauseLoop = true;
            integralErrorVoltageRA = 0;
            integralErrorVoltageDEC = 0;
            prevErrorVoltageRA = 0;
            prevErrorVoltageDEC = 0;
            curErrorVoltageRA = 0;
            curErrorVoltageDEC = 0;
            exitMotor(1);
            exitMotor(2);
            disableMotor(1);
            disableMotor(2);
            std::sprintf(logString, "Image out of Sensor, Please readjust and press c");
            log(logString, ERROR);
            while(pauseLoop == true) {
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
            std::sprintf(logString, "Failed to get Voltage");
            log(logString, ERROR);
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
            std::sprintf(logString, "Mean Voltage RA:  %.2f", currMeanRA);
            log(logString, INFO);
            std::sprintf(logString, "Mean Voltage DEC:  %.2f", currMeanDEC);
            log(logString, INFO);
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
                RA = abs(workerParams.maxVoltageChangeInMiliVoltsPerSecRA);
                if(prevRaDirection == raDirection)
                    quitCountRA += 1;
            }
            else {
                quitCountRA = 0;
            }
            if (DEC > abs(workerParams.maxVoltageChangeInMiliVoltsPerSecDEC)) {
                DEC = abs(workerParams.maxVoltageChangeInMiliVoltsPerSecDEC);
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
            int64 raCounts = raDirection * RA;
            std::sprintf(logString, "Applying counts RA:  %d", raCounts);
            log(logString, DEBUG);
            statusSetMotorCount = setMotorCount(raMotorNum, raDirection, RA);
            if (statusSetMotorCount != 0) {
                free(readArray);
                workerParams.statusWorker = -2;
                return -2;
            }
            int64 decCounts = decDirection * RA;
            std::sprintf(logString, "Applying counts DEC:  %d", decCounts);
            log(logString, DEBUG);
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
    std::sprintf(logString, "Sample Input Mode: %d", mode);
    log(logString, ERROR);
    std::sprintf(logString, "Device Name: %s", deviceName.c_str());
    log(logString, DEBUG);
    std::sprintf(logString, "Enter the Slope of the RA calibration line equation:");
    log(logString, PROMPT);
    getline(std::cin, slopeString);
    std::sprintf(logString, "Enter the Constant of the RA calibration line equation:");
    log(logString, PROMPT);
    getline(std::cin, constantString);
    raSlope = atof(slopeString.c_str());
    raConstant = atof(constantString.c_str());
    raReferenceVoltage = atof(referenceVoltageString.c_str());
    std::sprintf(logString, "Enter the Slope of the DEC calibration line equation:");
    log(logString, PROMPT);
    getline(std::cin, slopeString);
    std::sprintf(logString, "Enter the Constant of the DEC calibration line equation:");
    log(logString, PROMPT);
    getline(std::cin, constantString);
    decSlope = atof(slopeString.c_str());
    decConstant = atof(constantString.c_str());
    decReferenceVoltage = atof(referenceVoltageString.c_str());
    std::sprintf(logString, "Enter the COM Port number: ");
    log(logString, PROMPT);
    getline(std::cin, input);
    comPortNum = std::atoi(input.c_str());
    int status = CreateControllerConnection(comPortNum);
    if (status != 0) {
        std::sprintf(logString, "Unable to connect to motor");
        log(logString, ERROR);
        return -1;
    }
    int statusMotorEnable = enableMotor(raMotorNum);
    if (statusMotorEnable != 0) {
        std::sprintf(logString, "Failed to Enable RA Motor");
        log(logString, ERROR);
        closeControllerConnection();
        return -2;
    }
    statusMotorEnable = enableMotor(decMotorNum);
    if (statusMotorEnable != 0) {
        std::sprintf(logString, "Failed to Enable DEC Motor");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        closeControllerConnection();
        return -2;
    }
    std::sprintf(logString, "Enter the Frequency of RA motor:");
    log(logString, PROMPT);
    getline(std::cin, frequencyString);
    raFrequency = atoi(frequencyString.c_str());
    std::sprintf(logString, "Enter the Frequency of DEC motor:");
    log(logString, PROMPT);
    getline(std::cin, frequencyString);
    decFrequency = atoi(frequencyString.c_str());
    int statusMotorFrequency = setMotorFrequency(raMotorNum, raFrequency);
    if (statusMotorFrequency != 0) {
        std::sprintf(logString, "Failed to Set RA Motor Frequency");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -3;
    }
    statusMotorFrequency = setMotorFrequency(decMotorNum, decFrequency);
    if (statusMotorFrequency != 0) {
        std::sprintf(logString, "Failed to Set DEC Motor Frequency");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -3;
    }
    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::sprintf(logString, "Failed to Create DaQ Task");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        return -4;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName.c_str());
    if (statusInitDaQChain != 0) {
        std::sprintf(logString, "Failed to init DaQ init chain");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        clearDAQTask();
        return -5;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::sprintf(logString, "Failed to Start DaQ Task");
        log(logString, ERROR);
        disableMotor(raMotorNum);
        disableMotor(decMotorNum);
        closeControllerConnection();
        clearDAQTask();
        return -6;
    }
    std::sprintf(logString, "Enter the number of voltage samples:");
    log(logString, PROMPT);
    getline(std::cin, numberOfVoltageSamplesString);
    numberOfVoltageSamples = atoi(numberOfVoltageSamplesString.c_str());
    std::sprintf(logString, "Enter the loop update time in seconds:");
    log(logString, PROMPT);
    getline(std::cin, loopUpdateTimeInSecondsString);
    loopUpdateTimeInSeconds = atof(loopUpdateTimeInSecondsString.c_str());
    std::sprintf(logString, "Enter the polling time in miliseconds:");
    log(logString, PROMPT);
    getline(std::cin, pollingTimeInMiliSecondsString);
    pollingTimeInMiliSeconds = atof(pollingTimeInMiliSecondsString.c_str());
    std::sprintf(logString, "Enter the max permissible counts change per second in RA:");
    log(logString, PROMPT);
    getline(std::cin, maxVoltageChangeInMiliVoltsPerSecString);
    maxVoltageChangeInMiliVoltsPerSecRA = atoi(maxVoltageChangeInMiliVoltsPerSecString.c_str());
    std::sprintf(logString, "Enter the max permissible counts change per second in DEC:");
    log(logString, PROMPT);
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
        std::sprintf(logString, "Enter 1 to give your own kp, ki, kd.");
        log(logString, PROMPT);
        getline(std::cin, input);
        int choice = atoi(input.c_str());
        if (choice == 1) {
            std::sprintf(logString, "Enter the value of Kp:");
            log(logString, PROMPT);
            getline(std::cin, input);
            workerParams.kp = atof(input.c_str());
            std::sprintf(logString, "Enter the value of Ki:");
            log(logString, PROMPT);
            getline(std::cin, input);
            workerParams.ki = atof(input.c_str());
            std::sprintf(logString, "Enter the value of Kd:");
            log(logString, PROMPT);
            getline(std::cin, input);
            workerParams.kd = atof(input.c_str());
        }
        else{
            workerParams.kp = 1.015;
            workerParams.ki = 0.004;
            workerParams.kd = 0.11;
        }
    }
    std::sprintf(logString, "Kp: %.2f Ki: %.2f Kd: %.2f", workerParams.kp, workerParams.ki, workerParams.kd);
    log(logString, INFO);
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
            std::sprintf(logString, "Failed to get Voltage");
            log(logString, ERROR);
            free(readArray);
            workerParams.statusWorker = -1;
            return -1;
        }
        getMeanRAAndDEC(readArray, readArraySize, samplesReadPerChannel, &meanRA, &meanDEC);
        std::sprintf(logString, "Mean RA: %.2f", meanRA);
        log(logString, INFO);
        std::sprintf(logString, "Mean DEC: %.2f", meanDEC);
        log(logString, INFO);
        while (1) {
            std::sprintf(logString, "Accept as Reference Voltages?y/n");
            log(logString, PROMPT);
            getline(std::cin, input);
            if (input[0] == 'Y' || input[0] == 'y') {
                inputAccepted = true;
                workerParams.raReferenceVoltage = meanRA;
                workerParams.decReferenceVoltage = meanDEC;
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
    std::sprintf(logString, "Press q to quit close loop operation.");
    log(logString, PROMPT);
    while(slopeString[0] != 'q' ) {
        if (pauseLoop == true && (slopeString[0] == 'c' || slopeString[0]== 'C')) {
            pauseLoop = false;
            enableMotor(1);
            enableMotor(2);
        }
        getline(std::cin, slopeString);
    }
    CloseHandle(myHandle);
    std::sprintf(logString, "Finishing the Closed Loop Process.");
    log(logString, PROMPT);
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
    std::sprintf(logString, "Samples Input Mode: %d", mode);
    log(logString, INFO);
    int comPortNum, frequency, count, step, direction, numOfVoltageSamples;
    int64 readArraySize;
    int32 samplesReadPerChannel = 0;
    std::string input, numOfVoltageSamplesString;
    std::sprintf(logString, "Enter the COM Port number: ");
    log(logString, PROMPT);
    getline(std::cin, input);
    comPortNum = std::atoi(input.c_str());
    int status = CreateControllerConnection(comPortNum);
    if (status != 0) {
        std::sprintf(logString, "Unable to connect to motor");
        log(logString, ERROR);
        std::cout<<"Unable to connect to motor"<<std::endl;
        return -1;
    }
    std::string frequencyString;
    std::string countString;
    std::string stepsString;
    std::string directionString;
    std::sprintf(logString, "Enter the raFrequency of motor: ");
    log(logString, PROMPT);
    getline(std::cin, frequencyString);
    std::sprintf(logString, "Enter the total counts to move: ");
    log(logString, PROMPT);
    getline(std::cin, countString);
    std::sprintf(logString, "Enter the step size in counts: ");
    log(logString, PROMPT);
    getline(std::cin, stepsString);
    std::sprintf(logString, "Enter the motor direction: ");
    log(logString, PROMPT);
    getline(std::cin, directionString);
    frequency = std::atoi(frequencyString.c_str());
    count = std::atoi(countString.c_str());
    step = std::atoi(stepsString.c_str());
    direction = std::atoi(directionString.c_str());
    int statusMotorEnable = enableMotor(motorNum);
    if (statusMotorEnable != 0) {
        std::sprintf(logString, "Failed to Enable Motor");
        log(logString, ERROR);
        closeControllerConnection();
        return -2;
    }
    int statusMotorFrequency = setMotorFrequency(motorNum, frequency);
    if (statusMotorFrequency != 0) {
        std::sprintf(logString, "Failed to Set Motor Frequency");
        log(logString, ERROR);
        disableMotor(motorNum);
        closeControllerConnection();
        return -3;
    }
    int statusDaQTask = createDAQTask();
    if (statusDaQTask != 0) {
        std::sprintf(logString, "Failed to Create DaQ Task");
        log(logString, ERROR);
        disableMotor(motorNum);
        closeControllerConnection();
        return -4;
    }
    int statusInitDaQChain = initDAQAIChan(deviceName);
    if (statusInitDaQChain != 0) {
        std::sprintf(logString, "Failed to init DaQ chain");
        log(logString, ERROR);
        disableMotor(motorNum);
        closeControllerConnection();
        clearDAQTask();
        return -5;
    }
    int statusStartDaQTask = startDAQTask();
    if (statusStartDaQTask != 0) {
        std::sprintf(logString, "Failed to Start DaQ Task");
        log(logString, ERROR);
        disableMotor(motorNum);
        closeControllerConnection();
        clearDAQTask();
        return -6;
    }
    std::sprintf(logString, "Enter number of Voltage Samples to Get:");
    log(logString, PROMPT);
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
            std::sprintf(logString, "Failed to get Voltage");
            log(logString, ERROR);
            exitMotor(motorNum);
            disableMotor(motorNum);
            closeControllerConnection();
            stopDAQTask();
            clearDAQTask();
            free(readArray);
            return -7;
        }
        char data[100000];
        char data_point[100];
        _itoa_s(totalSteps, data, 10);
        for (int i=0; i <  readArraySize; i++) {
            std::sprintf(data_point, "  %.2f", readArray[i] * 1000);
            std::strcat(data, data_point);
        }
        log(data, INFO);
        statusSetMotorCount = setMotorCount(motorNum, direction, step);
        if (statusSetMotorCount != 0) {
            std::sprintf(logString, "Failed to move Motor");
            log(logString, ERROR);
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
        std::sprintf(logString, "Failed to move Motor back to initial position");
        log(logString, ERROR);
        exitMotor(motorNum);
        disableMotor(motorNum);
        closeControllerConnection();
        stopDAQTask();
        clearDAQTask();
        return -9;
    }
    Sleep(goBackTime * 2);
    std::sprintf(logString, "Test Successfully Completed.");
    log(logString, PROMPT);
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