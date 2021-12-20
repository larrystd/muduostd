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


if(BOOSTPO_LIBRARY)
  add_executable(memcached_debug Item.cc MemcacheServer.cc Session.cc server.cc)
  target_link_libraries(memcached_debug muduo_http inspect_source muduo_net boost_program_options  muduo_base pthread)
endif()

add_executable(memcached_footprint Item.cc MemcacheServer.cc Session.cc footprint_test.cc)
target_link_libraries(memcached_footprint inspect_source muduo_http muduo_net  muduo_base pthread)

if(TCMALLOC_INCLUDE_DIR AND TCMALLOC_LIBRARY)
  set_target_properties(memcached_footprint PROPERTIES COMPILE_FLAGS "-DHAVE_TCMALLOC")
  if(BOOSTPO_LIBRARY)
    set_target_properties(memcached_debug PROPERTIES COMPILE_FLAGS "-DHAVE_TCMALLOC")
  endif()
endif()

set(serversrc
  server.cc 
  Item.cc 
  MemcacheServer.cc 
  Session.cc)

add_executable(server ${serversrc}) 
target_link_libraries(server inspect_source muduo_http muduo_net  muduo_base pthread ${Boost_LIBRARIES})



