cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(NANE)

#dependancies
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Catch2 REQUIRED)

#where source code is kept
include_directories(../NANE/src)
file(GLOB_RECURSE SOURCES "../NANE/src/*.cpp")

#NANE executable
add_executable(NANE ${SOURCES})
target_link_libraries(NANE ${SDL2_LIBRARIES})

#unit test executable
file(GLOB_RECURSE TEST_SOURCES "../NANE/test/*.cpp")
add_executable(unit_tests ${TEST_SOURCES})
target_include_directories(unit_tests PUBLIC ../NANE/test)
target_link_libraries(unit_tests Catch2::Catch2)


if(DEFINED LOG_SERVER_IP)
    add_definitions(-D LOG_SERVER_IP=\\\"${LOG_SERVER_IP}\\\")
endif()

add_definitions(-D__DESKTOP__)