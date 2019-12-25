cmake_minimum_required(VERSION 3.16)

project(NANE)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

include_directories(../NANE/src)
file(GLOB_RECURSE SOURCES "../NANE/src/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

if(DEFINED LOG_SERVER_IP)
    add_definitions(-D LOG_SERVER_IP=\\\"${LOG_SERVER_IP}\\\")
endif()

add_definitions(-D__DESKTOP__)
add_executable(NANE ${SOURCES})
target_link_libraries(NANE ${SDL2_LIBRARIES})