//
// Created by harsh on 1/12/21.
//

#ifndef NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H
#define NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H
#include <iostream>
#include <windows.h>
#include <string>
#include <stdlib.h>
#include "utilheaders.h"

extern HANDLE serialHandle;

int createSerialConnection(int comPortNum, int baudRate, int byteSize, int stopBits, int parity);

int setTimeout(int readIntervalTimeout, int readTotalTimeoutConstant, int readTotalTimeoutMultiplier, int writeTotalTimeoutConstant, int writeTotalTimeoutMultiplier);

int closeSerialConnection();

int writeToPort(std::string dataWord);

int readFromPort(char *dataWord, DWORD nNumberOfBytesToRead, DWORD* NumberOfBytesRead);

int testSerialConnection();

#endif //NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H
