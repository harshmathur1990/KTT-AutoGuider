# CMAKE generated file: DO NOT EDIT!
# Generated by "NMake Makefiles" Generator, CMake Version 3.14

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

!IF "$(OS)" == "Windows_NT"
NULL=
!ELSE
NULL=nul
!ENDIF
SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "C:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe"

# The command to remove a file.
RM = "C:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\Users\IIAP\CLionProjects\FPVoltageController

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug

# Include any dependencies generated for this target.
include CMakeFiles\FPVoltageController.dir\depend.make

# Include the progress variables for this target.
include CMakeFiles\FPVoltageController.dir\progress.make

# Include the compile flags for this target's objects.
include CMakeFiles\FPVoltageController.dir\flags.make

CMakeFiles\FPVoltageController.dir\main.cpp.obj: CMakeFiles\FPVoltageController.dir\flags.make
CMakeFiles\FPVoltageController.dir\main.cpp.obj: ..\main.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/FPVoltageController.dir/main.cpp.obj"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\FPVoltageController.dir\main.cpp.obj /FdCMakeFiles\FPVoltageController.dir\ /FS -c C:\Users\IIAP\CLionProjects\FPVoltageController\main.cpp
<<

CMakeFiles\FPVoltageController.dir\main.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FPVoltageController.dir/main.cpp.i"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\FPVoltageController.dir\main.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\IIAP\CLionProjects\FPVoltageController\main.cpp
<<

CMakeFiles\FPVoltageController.dir\main.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FPVoltageController.dir/main.cpp.s"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\FPVoltageController.dir\main.cpp.s /c C:\Users\IIAP\CLionProjects\FPVoltageController\main.cpp
<<

CMakeFiles\FPVoltageController.dir\daqutils.cpp.obj: CMakeFiles\FPVoltageController.dir\flags.make
CMakeFiles\FPVoltageController.dir\daqutils.cpp.obj: ..\daqutils.cpp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object CMakeFiles/FPVoltageController.dir/daqutils.cpp.obj"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoCMakeFiles\FPVoltageController.dir\daqutils.cpp.obj /FdCMakeFiles\FPVoltageController.dir\ /FS -c C:\Users\IIAP\CLionProjects\FPVoltageController\daqutils.cpp
<<

CMakeFiles\FPVoltageController.dir\daqutils.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/FPVoltageController.dir/daqutils.cpp.i"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe > CMakeFiles\FPVoltageController.dir\daqutils.cpp.i @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E C:\Users\IIAP\CLionProjects\FPVoltageController\daqutils.cpp
<<

CMakeFiles\FPVoltageController.dir\daqutils.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/FPVoltageController.dir/daqutils.cpp.s"
	C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\cl.exe @<<
 /nologo /TP $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) /FoNUL /FAs /FaCMakeFiles\FPVoltageController.dir\daqutils.cpp.s /c C:\Users\IIAP\CLionProjects\FPVoltageController\daqutils.cpp
<<

# Object files for target FPVoltageController
FPVoltageController_OBJECTS = \
"CMakeFiles\FPVoltageController.dir\main.cpp.obj" \
"CMakeFiles\FPVoltageController.dir\daqutils.cpp.obj"

# External object files for target FPVoltageController
FPVoltageController_EXTERNAL_OBJECTS =

FPVoltageController.exe: CMakeFiles\FPVoltageController.dir\main.cpp.obj
FPVoltageController.exe: CMakeFiles\FPVoltageController.dir\daqutils.cpp.obj
FPVoltageController.exe: CMakeFiles\FPVoltageController.dir\build.make
FPVoltageController.exe: CMakeFiles\FPVoltageController.dir\objects1.rsp
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable FPVoltageController.exe"
	"C:\Program Files\JetBrains\CLion 2019.1.2\bin\cmake\win\bin\cmake.exe" -E vs_link_exe --intdir=CMakeFiles\FPVoltageController.dir --rc=C:\PROGRA~2\WI3CF2~1\10\bin\100171~1.0\x86\rc.exe --mt=C:\PROGRA~2\WI3CF2~1\10\bin\100171~1.0\x86\mt.exe --manifests  -- C:\PROGRA~2\MIB055~1\2017\COMMUN~1\VC\Tools\MSVC\1414~1.264\bin\Hostx86\x86\link.exe /nologo @CMakeFiles\FPVoltageController.dir\objects1.rsp @<<
 /out:FPVoltageController.exe /implib:FPVoltageController.lib /pdb:C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug\FPVoltageController.pdb /version:0.0  /machine:X86 /debug /INCREMENTAL /subsystem:console kernel32.lib user32.lib gdi32.lib winspool.lib shell32.lib ole32.lib oleaut32.lib uuid.lib comdlg32.lib advapi32.lib 
<<

# Rule to build all files generated by this target.
CMakeFiles\FPVoltageController.dir\build: FPVoltageController.exe

.PHONY : CMakeFiles\FPVoltageController.dir\build

CMakeFiles\FPVoltageController.dir\clean:
	$(CMAKE_COMMAND) -P CMakeFiles\FPVoltageController.dir\cmake_clean.cmake
.PHONY : CMakeFiles\FPVoltageController.dir\clean

CMakeFiles\FPVoltageController.dir\depend:
	$(CMAKE_COMMAND) -E cmake_depends "NMake Makefiles" C:\Users\IIAP\CLionProjects\FPVoltageController C:\Users\IIAP\CLionProjects\FPVoltageController C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug C:\Users\IIAP\CLionProjects\FPVoltageController\cmake-build-debug\CMakeFiles\FPVoltageController.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles\FPVoltageController.dir\depend

