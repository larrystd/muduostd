cmake_minimum_required(VERSION 2.6)

project(cdns C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

include_directories(/home/larry/myproject/myc++proj/muduostd)
link_directories(/home/larry/myproject/myc++proj/muduostd/lib)
find_package(Boost COMPONENTS program_options REQUIRED)
INCLUDE_DIRECTORIES( ${Boost_INCLUDE_DIR} )


add_executable(memcached_bench bench.cc)
target_link_libraries(memcached_bench inspect_source muduo_http muduo_net  muduo_base pthread ${Boost_LIBRARIES})