cmake_minimum_required(VERSION 2.6)

project(cdns C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

include_directories(/home/larry/myproject/myc++proj/muduostd)
link_directories(/home/larry/myproject/myc++proj/muduostd/lib)


add_library(muduo_cdns Resolver.cc)
target_link_libraries(muduo_cdns muduo_net muduo_base pthread)
target_link_libraries(muduo_cdns cares)

install(TARGETS muduo_cdns DESTINATION lib)
install(FILES Resolver.h DESTINATION include)


add_executable(cdns dns.cc)
target_link_libraries(cdns muduo_cdns)
install(TARGETS cdns DESTINATION bin)
