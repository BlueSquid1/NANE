cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

project(NANE)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

find_package(Catch2 REQUIRED)

include_directories(../NANE/src)
file(GLOB_RECURSE SOURCES "../NANE/src/*.cpp")

add_executable(NANE ${SOURCES})
target_link_libraries(NANE ${SDL2_LIBRARIES})

add_executable(unit_tests ../NANE/test/test_main.cpp ../NANE/test/NES/ROM/INesParserTest.cpp)
target_link_libraries(unit_tests Catch2::Catch2)
# catch_discover_tests(unit_tests WORKING_DIRECTORY ../NANE/test)


if(DEFINED LOG_SERVER_IP)
    add_definitions(-D LOG_SERVER_IP=\\\"${LOG_SERVER_IP}\\\")
endif()

add_definitions(-D__DESKTOP__)