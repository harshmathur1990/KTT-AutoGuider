//
// Created by harsh on 1/12/21.
//
#include "serialconnection.h"

HANDLE serialHandle;

int createSerialConnection(int comPortNum, int baudRate, int byteSize, int stopBits, int parity){
    std::string comPortStr = "\\\\.\\COM";
    comPortStr.append(std::to_string(comPortNum));
    serialHandle = CreateFile(comPortStr.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (serialHandle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    DCB serialParams = { 0 };
    serialParams.DCBlength = sizeof(serialParams);

    GetCommState(serialHandle, &serialParams);
    serialParams.BaudRate = baudRate;
    serialParams.ByteSize = byteSize;
    serialParams.StopBits = stopBits;
    serialParams.Parity = parity;
    bool status = SetCommState(serialHandle, &serialParams);
    return status == true?0:-2;
}

int setTimeout(int readIntervalTimeout, int readTotalTimeoutConstant, int readTotalTimeoutMultiplier, int writeTotalTimeoutConstant, int writeTotalTimeoutMultiplier) {
    COMMTIMEOUTS timeout = { 0 };
    timeout.ReadIntervalTimeout = readIntervalTimeout;
    timeout.ReadTotalTimeoutConstant = readTotalTimeoutConstant;
    timeout.ReadTotalTimeoutMultiplier = readTotalTimeoutMultiplier;
    timeout.WriteTotalTimeoutConstant = writeTotalTimeoutConstant;
    timeout.WriteTotalTimeoutMultiplier = writeTotalTimeoutMultiplier;

    bool status = SetCommTimeouts(serialHandle, &timeout);
    return status == true?0:-1;
}

int closeSerialConnection() {
    bool status = CloseHandle(serialHandle);
    return status == true?0:-1;
}

int writeToPort(const char dataWord[]) {
    DWORD dwBytesToWrite = (DWORD)strlen(dataWord);
    DWORD dwBytesWritten = 0;
    bool bErrorFlag = WriteFile(
            serialHandle,           // open file handle
            dataWord,      // start of data to write
            dwBytesToWrite,  // number of bytes to write
            &dwBytesWritten, // number of bytes that were written
            NULL);            // no overlapped structure
    return bErrorFlag == true?0:-1;
}