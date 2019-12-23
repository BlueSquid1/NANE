cmake_minimum_required(VERSION 3.16)

project(log_server)
 
#Bring the headers, such as Student.h into the project
include_directories(source)

file(GLOB_RECURSE SOURCES "source/*.cpp")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(server ${SOURCES})