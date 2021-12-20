cmake_minimum_required(VERSION 2.6)

project(inspect C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})
#set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
#set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)


set(source
  Inspector.cc
  PerformanceInspector.cc
  ProcessInspector.cc
  SystemInspector.cc
  )

add_library(inspect_source ${source})

install(TARGETS inspect_source DESTINATION lib)

set(header
  Inspector.h
  PerformanceInspector.h
  ProcessInspector.h
  SystemInspector.h
  )
install(FILES ${header} DESTINATION include)


find_package(Boost REQUIRED)
include_directories(/home/larry/myproject/myc++proj/muduostd)
include_directories(/home/larry/myproject/myc++proj/muduostd/http)
include_directories(/home/larry/myproject/myc++proj/muduostd/inspect)
link_directories(/home/larry/myproject/myc++proj/muduostd/build/lib)
#link_libraries(muduo_base)
#link_libraries(muduo_net)
#link_libraries(pthread)

set(dep_lib muduo_net muduo_base muduo_http pthread)
find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(inspector_test Inspector_test.cc ${source})
target_link_libraries(inspector_test ${dep_lib})
