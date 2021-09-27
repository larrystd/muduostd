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
include_directories(/home/larry/myproject/myc++proj/muduostd)
include_directories(/usr/local/curl/include)
link_directories(/usr/local/curl/lib)
link_directories(/home/larry/myproject/myc++proj/muduostd/build/lib)

add_library(muduo_curl Curl.cc)
target_link_libraries(muduo_curl muduo_net muduo_base pthread curl)
target_link_libraries(muduo_curl curl)

install(TARGETS muduo_curl DESTINATION lib)
install(FILES Curl.h DESTINATION include/muduo/curl)

add_executable(mcurl mcurl.cc)
target_link_libraries(mcurl muduo_curl muduo_base pthread)

add_executable(curl_download download.cc)
target_link_libraries(curl_download muduo_curl muduo_base pthread)

