//
// Created by harsh on 1/12/21.
//

#include "ra.h"

int testRa() {
    testChannel("Dev1/ai2");
    return 0;
}

int monitorRA() {
    monitorChannel("Dev1/ai2", 1);
    return 0;
}