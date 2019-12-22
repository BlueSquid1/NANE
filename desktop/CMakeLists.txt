cmake_minimum_required(VERSION 3.16)

project(log_server)
 
#Bring the headers, such as Student.h into the project
include_directories(source)

file(GLOB_RECURSE SOURCES "source/*.cpp")
 
add_executable(server ${SOURCES})