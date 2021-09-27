#add_executable(pingpong_client client.cc)
#target_link_libraries(pingpong_client muduo_net)

#add_executable(pingpong_server server.cc)
#target_link_libraries(pingpong_server muduo_net)

#add_executable(pingpong_bench bench.cc)
#target_link_libraries(pingpong_bench muduo_net)



cmake_minimum_required(VERSION 2.6)

project(pingpong C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
#enable_testing()

set(CXX_FLAGS
 -g
 # -DVALGRIND
 -DCHECK_PTHREAD_RETURN_VALUE
 -D_FILE_OFFSET_BITS=64
 -Wall
 -Wextra
 -Werror
 -Wconversion
 -Wno-unused-parameter
 -Wold-style-cast
 -Woverloaded-virtual
 -Wpointer-arith
 -Wshadow
 -Wwrite-strings
 -march=native
 # -MMD
 -std=c++11
 -rdynamic
 )

set(CMAKE_CXX_FLAGS_DEBUG "-O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
# make install 命令安装的位置

#install(FILES ${HEADERS} DESTINATION include/muduo/net/http)
#find_package(Threads REQUIRED)
include_directories(/home/larry/myproject/myc++proj/muduostd)
link_directories(/home/larry/myproject/myc++proj/muduostd/build/lib)

#include_directories(${Boost_INCLUDE_DIRS})


add_executable(pingpong_client client.cc)
target_link_libraries(pingpong_client muduo_base pthread rt muduo_net)

add_executable(pingpong_server server.cc)
target_link_libraries(pingpong_server muduo_base pthread rt muduo_net)

add_executable(pingpong_bench bench.cc)
target_link_libraries(pingpong_bench  muduo_net muduo_base pthread rt )


# add_subdirectory(tests)
