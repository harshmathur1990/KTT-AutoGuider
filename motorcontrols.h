//
// Created by harsh on 1/12/21.
//

#ifndef NIDAQVOLTAGEGENERATOR_MOTORCONTROLS_H
#define NIDAQVOLTAGEGENERATOR_MOTORCONTROLS_H

#include <iostream>

#include "serialconnection.h"

int CreateControllerConnection(int ComPortNum);

int closeControllerConnection();

int testMotor();

int setMotorFrequency(int motorNum, int frequency);

int setMotorCount(int motorNum, int direction, int counts);

int exitMotor(int motorNum);

#endif //NIDAQVOLTAGEGENERATOR_MOTORCONTROLS_H
