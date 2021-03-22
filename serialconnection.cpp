//
// Created by harsh on 1/12/21.
//
#include "serialconnection.h"

HANDLE serialHandle;

int createSerialConnection(int comPortNum, int baudRate, int byteSize, int stopBits, int parity){
    std::string comPortStr = std::string("\\\\.\\COM");
    comPortStr.append(std::to_string(comPortNum));
    std::cout<<"COM Port: "<<comPortStr<<std::endl;
    serialHandle = CreateFile(comPortStr.c_str(), GENERIC_READ | GENERIC_WRITE, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (serialHandle == INVALID_HANDLE_VALUE) {
        return -1;
    }

    char byteSizeBuffer[2], stopBitsBuffer[2], parityBuffer[2];
    _itoa_s(byteSize, byteSizeBuffer, 10);
    _itoa_s(stopBits, stopBitsBuffer, 10);
    _itoa_s(parity, parityBuffer, 10);
    DCB serialParams;
    memset(&serialParams, 0, sizeof(DCB));

    bool statusGetCommState = GetCommState(serialHandle, &serialParams);
    std::cout<<"statusGetCommState: "<<statusGetCommState<<std::endl;
    serialParams.BaudRate = (DWORD)baudRate;
    serialParams.ByteSize = 8;
    serialParams.StopBits = ONESTOPBIT;
    serialParams.Parity = NOPARITY;
    bool statusSetCommState = SetCommState(serialHandle, &serialParams);
    return statusSetCommState == true?0:-2;
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

int writeToPort(std::string dataWord) {
    const char *dataWordStr;
    dataWord.insert(0, std::string("<"));
    dataWord.append((">\r"));
    dataWordStr = dataWord.c_str();
    std::sprintf(logString, "Command to Motor:  %s", dataWordStr);
    log(logString, DEBUG);
    DWORD dwBytesToWrite = (DWORD)strlen(dataWordStr);
    DWORD dwBytesWritten = 0;
    bool bErrorFlag = WriteFile(
            serialHandle,           // open file handle
            dataWordStr,      // start of data to write
            dwBytesToWrite,  // number of bytes to write
            &dwBytesWritten, // number of bytes that were written
            NULL);            // no overlapped structure
    return bErrorFlag == true?0:-1;
}

int readFromPort(char *dataWord, DWORD NumberOfBytesToRead, DWORD* NumberOfBytesRead) {
    bool bErrorFlag = ReadFile(
            serialHandle,           // open file handle
            dataWord,      // start of data to write
            NumberOfBytesToRead,  // number of bytes to write
            NumberOfBytesRead, // number of bytes that were written
            NULL);            // no overlapped structure
    return bErrorFlag == true?0:-1;
}

int testSerialConnection() {
    std::string input;
    std::string testString;
    while (1) {
        std::sprintf(logString, "Enter e for exit, m for mode switch or a COM port :");
        log(logString, PROMPT);
        getline(std::cin, input);
        if (input[0] == 'm' || input[0] == 'M') {
            break;
        }
        if (input[0] == 'e' || input[0] == 'E') {
            exit(0);
        }
        int comPort = std::atoi(input.c_str());
        int status = createSerialConnection(
                comPort,
                9600,
                8,
                1,
                0
        );
        std::sprintf(logString, "Status of COM Port opening: %d", status);
        log(logString, ERROR);
        if (status == 0) {
            std::sprintf(logString, "Enter test String:");
            log(logString, PROMPT);
            getline(std::cin, testString);
            writeToPort(testString);
            DWORD dwBytesToRead = (DWORD)strlen(testString.c_str());
            DWORD dwBytesRead;
            char* output = NULL;
            readFromPort(output, dwBytesToRead, &dwBytesRead);
            std::sprintf(logString, "String Read: %s", output);
            log(logString, INFO);
        }
        closeSerialConnection();
    }
    return 0;
}