cmake_minimum_required(VERSION 2.6)

project(codec C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(CMAKE_INSTALL_PREFIX ${PROJECT_BINARY_DIR})

include_directories(/home/larry/myproject/myc++proj/muduostd)
include_directories(/usr/local/include)

link_directories(/home/larry/myproject/myc++proj/muduostd/lib)
link_directories(/usr/local/lib)

set(protobuf_codec  muduo_net  muduo_protorpc muduo_protorpc_wire muduo_protobuf_codec  muduo_base libprotobuf.a pthread z)  # z压缩库

add_custom_command(OUTPUT echo.pb.cc echo.pb.h
  COMMAND protoc
  ARGS --cpp_out . ${CMAKE_CURRENT_SOURCE_DIR}/echo.proto -I${CMAKE_CURRENT_SOURCE_DIR}
  DEPENDS echo.proto)

set_source_files_properties(echo.pb.cc PROPERTIES COMPILE_FLAGS "-Wno-conversion -Wno-shadow")
include_directories(${PROJECT_BINARY_DIR})

add_library(echo_proto echo.pb.cc)
target_link_libraries(echo_proto protobuf pthread)

add_executable(protobuf_rpc_echo_client client.cc)
set_target_properties(protobuf_rpc_echo_client PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_rpc_echo_client echo_proto ${protobuf_codec})

add_executable(protobuf_rpc_echo_server server.cc)
set_target_properties(protobuf_rpc_echo_server PROPERTIES COMPILE_FLAGS "-Wno-error=shadow")
target_link_libraries(protobuf_rpc_echo_server echo_proto ${protobuf_codec})
