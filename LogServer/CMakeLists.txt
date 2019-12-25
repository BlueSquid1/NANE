cmake_minimum_required(VERSION 3.16)

project(LogServer)
 
#Bring the headers, such as Student.h into the project
include_directories(src)

file(GLOB_RECURSE SOURCES "src/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

add_executable(LogServer ${SOURCES})