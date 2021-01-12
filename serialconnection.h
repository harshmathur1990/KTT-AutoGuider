//
// Created by harsh on 1/12/21.
//

#ifndef NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H
#define NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H

#include <windows.h>
#include <string>

extern HANDLE serialHandle;

int createSerialConnection(int comPortNum, int baudRate, int byteSize, int stopBits, int parity);

int setTimeout(int readIntervalTimeout, int readTotalTimeoutConstant, int readTotalTimeoutMultiplier, int writeTotalTimeoutConstant, int writeTotalTimeoutMultiplier);

int closeSerialConnection();

int writeToPort(const char dataWord[]);

#endif //NIDAQVOLTAGEGENERATOR_MOTORCONTROL_H
