# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.21

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
CMAKE_COMMAND = /opt/software/software/CMake/3.21.1-GCCcore-11.2.0/bin/cmake

# The command to remove a file.
RM = /opt/software/software/CMake/3.21.1-GCCcore-11.2.0/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /homes/calebkmiecik/Project-2/hw2

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /homes/calebkmiecik/Project-2/hw2/build

# Include any dependencies generated for this target.
include CMakeFiles/dyn_array.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/dyn_array.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/dyn_array.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/dyn_array.dir/flags.make

CMakeFiles/dyn_array.dir/src/dyn_array.c.o: CMakeFiles/dyn_array.dir/flags.make
CMakeFiles/dyn_array.dir/src/dyn_array.c.o: ../src/dyn_array.c
CMakeFiles/dyn_array.dir/src/dyn_array.c.o: CMakeFiles/dyn_array.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/homes/calebkmiecik/Project-2/hw2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/dyn_array.dir/src/dyn_array.c.o"
	/opt/software/software/GCCcore/11.2.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/dyn_array.dir/src/dyn_array.c.o -MF CMakeFiles/dyn_array.dir/src/dyn_array.c.o.d -o CMakeFiles/dyn_array.dir/src/dyn_array.c.o -c /homes/calebkmiecik/Project-2/hw2/src/dyn_array.c

CMakeFiles/dyn_array.dir/src/dyn_array.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/dyn_array.dir/src/dyn_array.c.i"
	/opt/software/software/GCCcore/11.2.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /homes/calebkmiecik/Project-2/hw2/src/dyn_array.c > CMakeFiles/dyn_array.dir/src/dyn_array.c.i

CMakeFiles/dyn_array.dir/src/dyn_array.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/dyn_array.dir/src/dyn_array.c.s"
	/opt/software/software/GCCcore/11.2.0/bin/cc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /homes/calebkmiecik/Project-2/hw2/src/dyn_array.c -o CMakeFiles/dyn_array.dir/src/dyn_array.c.s

# Object files for target dyn_array
dyn_array_OBJECTS = \
"CMakeFiles/dyn_array.dir/src/dyn_array.c.o"

# External object files for target dyn_array
dyn_array_EXTERNAL_OBJECTS =

libdyn_array.a: CMakeFiles/dyn_array.dir/src/dyn_array.c.o
libdyn_array.a: CMakeFiles/dyn_array.dir/build.make
libdyn_array.a: CMakeFiles/dyn_array.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/homes/calebkmiecik/Project-2/hw2/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libdyn_array.a"
	$(CMAKE_COMMAND) -P CMakeFiles/dyn_array.dir/cmake_clean_target.cmake
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/dyn_array.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/dyn_array.dir/build: libdyn_array.a
.PHONY : CMakeFiles/dyn_array.dir/build

CMakeFiles/dyn_array.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/dyn_array.dir/cmake_clean.cmake
.PHONY : CMakeFiles/dyn_array.dir/clean

CMakeFiles/dyn_array.dir/depend:
	cd /homes/calebkmiecik/Project-2/hw2/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /homes/calebkmiecik/Project-2/hw2 /homes/calebkmiecik/Project-2/hw2 /homes/calebkmiecik/Project-2/hw2/build /homes/calebkmiecik/Project-2/hw2/build /homes/calebkmiecik/Project-2/hw2/build/CMakeFiles/dyn_array.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/dyn_array.dir/depend

