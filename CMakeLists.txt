cmake_minimum_required(VERSION 3.14)
project(FPVoltageController)

set(CMAKE_CXX_STANDARD 14)
set(INCLUDE_DIR "C:/Program Files (x86)/National Instruments/NI-DAQ/DAQmx ANSI C Dev/include")
set(HEADER_FILES "${INCLUDE_DIR}/NIDAQmx.h")
set(SOURCES main.cpp daqutils.cpp)
add_executable(FPVoltageController ${SOURCES} ${HEADER_FILES})
target_include_directories (FPVoltageController PRIVATE ${INCLUDE_DIR})