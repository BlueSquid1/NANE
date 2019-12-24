cmake_minimum_required(VERSION 3.16)

project(NES-Desktop)

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})


include_directories(../NES/source)
file(GLOB_RECURSE SOURCES "../NES/source/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_definitions(-D__DESKTOP__)
add_executable(NES-Desktop ${SOURCES})
target_link_libraries(NES-Desktop ${SDL2_LIBRARIES})