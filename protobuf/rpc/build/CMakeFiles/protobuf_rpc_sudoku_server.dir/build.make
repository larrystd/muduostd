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
CMAKE_SOURCE_DIR = /home/larry/myproject/myc++proj/muduostd/protobuf/rpc

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build

# Include any dependencies generated for this target.
include CMakeFiles/protobuf_rpc_sudoku_server.dir/depend.make

# Include the progress variables for this target.
include CMakeFiles/protobuf_rpc_sudoku_server.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/protobuf_rpc_sudoku_server.dir/flags.make

CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o: CMakeFiles/protobuf_rpc_sudoku_server.dir/flags.make
CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o: ../server.cc
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o"
	/usr/bin/c++  $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -o CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o -c /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/server.cc

CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.i"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/server.cc > CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.i

CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.s"
	/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/server.cc -o CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.s

# Object files for target protobuf_rpc_sudoku_server
protobuf_rpc_sudoku_server_OBJECTS = \
"CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o"

# External object files for target protobuf_rpc_sudoku_server
protobuf_rpc_sudoku_server_EXTERNAL_OBJECTS =

protobuf_rpc_sudoku_server: CMakeFiles/protobuf_rpc_sudoku_server.dir/server.cc.o
protobuf_rpc_sudoku_server: CMakeFiles/protobuf_rpc_sudoku_server.dir/build.make
protobuf_rpc_sudoku_server: libsudoku_proto.a
protobuf_rpc_sudoku_server: CMakeFiles/protobuf_rpc_sudoku_server.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking CXX executable protobuf_rpc_sudoku_server"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/protobuf_rpc_sudoku_server.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/protobuf_rpc_sudoku_server.dir/build: protobuf_rpc_sudoku_server

.PHONY : CMakeFiles/protobuf_rpc_sudoku_server.dir/build

CMakeFiles/protobuf_rpc_sudoku_server.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/protobuf_rpc_sudoku_server.dir/cmake_clean.cmake
.PHONY : CMakeFiles/protobuf_rpc_sudoku_server.dir/clean

CMakeFiles/protobuf_rpc_sudoku_server.dir/depend:
	cd /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /home/larry/myproject/myc++proj/muduostd/protobuf/rpc /home/larry/myproject/myc++proj/muduostd/protobuf/rpc /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build /home/larry/myproject/myc++proj/muduostd/protobuf/rpc/build/CMakeFiles/protobuf_rpc_sudoku_server.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/protobuf_rpc_sudoku_server.dir/depend
