cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(source_dir "${CMAKE_SOURCE_DIR}/../NANE")

project(NANE)

#dependancies
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${PROJECT_SOURCE_DIR}")
find_package(SDL2_ttf REQUIRED)
include_directories(${SDL2_TTF_INCLUDE_DIRS})

find_package(Catch2 REQUIRED)

#where source code is kept
include_directories(${source_dir}/src)
file(GLOB_RECURSE SOURCES 
    ${source_dir}/src/GameEngine/*.cpp 
    ${source_dir}/src/NES/*.cpp)

#NANE library
add_library(NANE_Lib STATIC ${SOURCES})
target_link_libraries(NANE_Lib ${SDL2_LIBRARIES})
target_link_libraries(NANE_Lib ${SDL2_TTF_LIBRARIES})

#NANE executable
add_executable(NANE ${source_dir}/src/main.cpp)
target_link_libraries(NANE NANE_Lib)

#unit test executable
file(GLOB_RECURSE TEST_SOURCES ${source_dir}/test/*.cpp)
add_executable(unit_tests ${TEST_SOURCES})
target_include_directories(unit_tests PUBLIC ${source_dir}/test)
target_link_libraries(unit_tests NANE_Lib)
target_link_libraries(unit_tests Catch2::Catch2)

#pass log server ip address to the compiled program if it is avaliable
if(DEFINED LOG_SERVER_IP)
    add_definitions(-D LOG_SERVER_IP=\\\"${LOG_SERVER_IP}\\\")
endif()

#created the predefined variable __DESKTOP__
add_definitions(-D__DESKTOP__)

#copy resource folder to target dirrectory
add_custom_command(TARGET NANE PRE_BUILD
                    COMMAND ${CMAKE_COMMAND} -E copy_directory
                       ${source_dir}/resources/ $<TARGET_FILE_DIR:NANE>)