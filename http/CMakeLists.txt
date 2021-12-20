cmake_minimum_required(VERSION 2.6)

project(http C CXX)

SET(CMAKE_BUILD_TYPE "Debug")
SET(CMAKE_CXX_FLAGS_DEBUG "$ENV{CXXFLAGS} -O0 -Wall -g2 -ggdb")
SET(CMAKE_CXX_FLAGS_RELEASE "$ENV{CXXFLAGS} -O3 -Wall")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_BINARY_DIR}/lib)
enable_testing()


FIND_PATH(myHeader mysql/mysql.h)
IF(myHeader)
INCLUDE_DIRECTORIES(${myHeader})
ENDIF(myHeader)

set(http_SRCS
  HttpServer.cc
  HttpResponse.cc
  HttpContext.cc
  )

add_library(muduo_http ${http_SRCS})


# make install 命令安装的位置
install(TARGETS muduo_http DESTINATION lib)
set(HEADERS
  HttpContext.h
  HttpRequest.h
  HttpResponse.h
  HttpServer.h
  )
#install(FILES ${HEADERS} DESTINATION include/muduo/net/http)
find_package(Boost REQUIRED)
#find_package(Threads REQUIRED)
include_directories(/home/larry/myproject/myc++proj/muduostd)
link_directories(/home/larry/myproject/myc++proj/muduostd/build/lib)
#link_libraries(muduo_base)
#link_libraries(muduo_net)
#link_libraries(pthread)

target_link_libraries(muduo_http muduo_net muduo_base pthread)

find_library(BOOSTTEST_LIBRARY NAMES boost_unit_test_framework)
include_directories(${Boost_INCLUDE_DIRS})

add_executable(httpserver_login tests/HttpServer_login.cc)
target_link_libraries(httpserver_login muduo_http mysqlclient)
