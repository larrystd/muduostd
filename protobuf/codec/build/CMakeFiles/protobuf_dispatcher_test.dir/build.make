# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.16

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/local/bin/cmake

# The command to remove a file.
RM = /usr/local/bin/cmake -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /home/larry/myproject/myc++proj/muduostd/protobuf/codec

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/larry/myproject/myc++proj/muduostd/protobuf/codec/build

# Include any dependencies generated for this target.
include CMakeFiles/protobuf_dispatcher_test.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/protobuf_dispatcher_test.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/protobuf_dispatcher_test.dir/flags.make

CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o: CMakeFiles/protobuf_dispatcher_test.dir/flags.make
CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o: ../dispatcher_test.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/larry/myproject/myc++proj/muduostd/protobuf/codec/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o -c /home/larry/myproject/myc++proj/muduostd/protobuf/codec/dispatcher_test.cc

CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/larry/myproject/myc++proj/muduostd/protobuf/codec/dispatcher_test.cc > CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.i

CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/larry/myproject/myc++proj/muduostd/protobuf/codec/dispatcher_test.cc -o CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.s

# Object files for target protobuf_dispatcher_test
protobuf_dispatcher_test_OBJECTS = \
"CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o"

# External object files for target protobuf_dispatcher_test
protobuf_dispatcher_test_EXTERNAL_OBJECTS =

protobuf_dispatcher_test: CMakeFiles/protobuf_dispatcher_test.dir/dispatcher_test.cc.o
protobuf_dispatcher_test: CMakeFiles/protobuf_dispatcher_test.dir/build.make
protobuf_dispatcher_test: libquery_proto.a
protobuf_dispatcher_test: CMakeFiles/protobuf_dispatcher_test.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/larry/myproject/myc++proj/muduostd/protobuf/codec/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable protobuf_dispatcher_test"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protobuf_dispatcher_test.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/protobuf_dispatcher_test.dir/build: protobuf_dispatcher_test

.PHONY : CMakeFiles/protobuf_dispatcher_test.dir/build

CMakeFiles/protobuf_dispatcher_test.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/protobuf_dispatcher_test.dir/cmake_clean.cmake
.PHONY : CMakeFiles/protobuf_dispatcher_test.dir/clean

CMakeFiles/protobuf_dispatcher_test.dir/depend:
	cd /home/larry/myproject/myc++proj/muduostd/protobuf/codec/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/larry/myproject/myc++proj/muduostd/protobuf/codec /home/larry/myproject/myc++proj/muduostd/protobuf/codec /home/larry/myproject/myc++proj/muduostd/protobuf/codec/build /home/larry/myproject/myc++proj/muduostd/protobuf/codec/build /home/larry/myproject/myc++proj/muduostd/protobuf/codec/build/CMakeFiles/protobuf_dispatcher_test.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/protobuf_dispatcher_test.dir/depend
