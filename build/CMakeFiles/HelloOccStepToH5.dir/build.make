# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.31

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/dragontesa/314/hello/HelloOccStepToH5

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/dragontesa/314/hello/HelloOccStepToH5/build

# Include any dependencies generated for this target.
include CMakeFiles/HelloOccStepToH5.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/HelloOccStepToH5.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/HelloOccStepToH5.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/HelloOccStepToH5.dir/flags.make

CMakeFiles/HelloOccStepToH5.dir/codegen:
.PHONY : CMakeFiles/HelloOccStepToH5.dir/codegen

CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o: CMakeFiles/HelloOccStepToH5.dir/flags.make
CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o: /home/dragontesa/314/hello/HelloOccStepToH5/WriteStepAttributeHdf5.cpp
CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o: CMakeFiles/HelloOccStepToH5.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --progress-dir=/home/dragontesa/314/hello/HelloOccStepToH5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o -MF CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o.d -o CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o -c /home/dragontesa/314/hello/HelloOccStepToH5/WriteStepAttributeHdf5.cpp

CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Preprocessing CXX source to CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.i"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/dragontesa/314/hello/HelloOccStepToH5/WriteStepAttributeHdf5.cpp > CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.i

CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green "Compiling CXX source to assembly CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.s"
	/usr/bin/g++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/dragontesa/314/hello/HelloOccStepToH5/WriteStepAttributeHdf5.cpp -o CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.s

# Object files for target HelloOccStepToH5
HelloOccStepToH5_OBJECTS = \
"CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o"

# External object files for target HelloOccStepToH5
HelloOccStepToH5_EXTERNAL_OBJECTS =

HelloOccStepToH5: CMakeFiles/HelloOccStepToH5.dir/WriteStepAttributeHdf5.cpp.o
HelloOccStepToH5: CMakeFiles/HelloOccStepToH5.dir/build.make
HelloOccStepToH5: CMakeFiles/HelloOccStepToH5.dir/compiler_depend.ts
HelloOccStepToH5: CMakeFiles/HelloOccStepToH5.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color "--switch=$(COLOR)" --green --bold --progress-dir=/home/dragontesa/314/hello/HelloOccStepToH5/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable HelloOccStepToH5"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/HelloOccStepToH5.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/HelloOccStepToH5.dir/build: HelloOccStepToH5
.PHONY : CMakeFiles/HelloOccStepToH5.dir/build

CMakeFiles/HelloOccStepToH5.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/HelloOccStepToH5.dir/cmake_clean.cmake
.PHONY : CMakeFiles/HelloOccStepToH5.dir/clean

CMakeFiles/HelloOccStepToH5.dir/depend:
	cd /home/dragontesa/314/hello/HelloOccStepToH5/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/dragontesa/314/hello/HelloOccStepToH5 /home/dragontesa/314/hello/HelloOccStepToH5 /home/dragontesa/314/hello/HelloOccStepToH5/build /home/dragontesa/314/hello/HelloOccStepToH5/build /home/dragontesa/314/hello/HelloOccStepToH5/build/CMakeFiles/HelloOccStepToH5.dir/DependInfo.cmake "--color=$(COLOR)"
.PHONY : CMakeFiles/HelloOccStepToH5.dir/depend

