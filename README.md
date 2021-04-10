# KTT-AutoGuider
This code is a software interface for the Auto Guider System deployed at KTT, Kodaikanal which consists of two SPOT-2D sensors and two stepper motors. NI-DaQX Board is used as ADC to measure the output voltage from SPOT-2D sensors, stepper motors are controlled through a controller board that interacts with the PC using the serial port.

# Requirements
1. Windows 7/8/10
2. Micrsoft Visual Studio with C++
3. NI DaQMX Library
4. Jetbrains Clion

#Installation
1. This is a cmake project, cmake build file is included, Clion should build this project automatically upon loading.
2. Executable files will be found in the folder "cmake-build-debug".

#Running the code:
1. Make sure the NI DaQ Board is connected and powered on.
2. Analog input 1 and Analog Input 2 will be connected to DEC and RA channels respectively.
3. It does not matter which channel is connected to which sensor as long as calibration is done accordingly.
4. Make sure the motor controller is connected through serial port.
5. Run the file FPVoltageController.
6. The executable is self explanatory and asks for prompts.
7. Please keep the image limb in outer segment of the sensors for best performance.
