cmake_minimum_required(VERSION 3.14)
project(FPVoltageController)

set(CMAKE_CXX_STANDARD 14)
include_directories("C:\\Program Files (x86)\\National Instruments\\NI-DAQ\\DAQmx ANSI C Dev\\include")
add_executable(FPVoltageController main.cpp daqutils.cpp)