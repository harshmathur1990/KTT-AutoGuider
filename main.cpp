#include <iostream>
#include <string>
#include <stdlib.h>
#include "daqutils.h"

//double VoltageData[1];

int main() {
    std::string input;
    int mode;
    std::cout << "Hello, World!" << std::endl;
    while (1) {
        std::cout << "Mode Select (Enter e for exit): " << std::endl;
        std::cout << "Enter 1 for RA test." << std::endl;
        std::cout << "Enter 2 for DEC test." << std::endl;
        std::cout << "Enter 3 for Motor test." << std::endl;
        std::cout << "Enter 4 for Close loop operation" << std::endl;
        getline(std::cin, input);
        if (input[0] == 'e' || input[0] == 'E') {
            break;
        }
        if (input[0] == 'm' || input[0] == 'M') {
            continue;
        }
        mode = atoi(token);
        switch (mode) {
            case 1:
                test_ra();
                break;
            case 2:
                test_dec();
                break;
            case 3:
                test_motor();
                break;
            case 4:
                closed_loop();
                break;
            default:
                break;
        }
    }
    return 0;
}