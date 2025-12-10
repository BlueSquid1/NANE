cmake_minimum_required(VERSION 3.2)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

set(source_dir "${CMAKE_SOURCE_DIR}/../NANE")

project(NANE-Web)
add_definitions(-D__DESKTOP__)

#dependancies
#find_package(SDL2 REQUIRED)
#include_directories(${SDL2_INCLUDE_DIRS})
#find_package(SDL2_ttf REQUIRED)
#include_directories(${SDL2_TTF_INCLUDE_DIRS})

#where source code is kept
include_directories(${source_dir}/src)
file(GLOB_RECURSE SOURCES 
    ${source_dir}/src/*.cpp
    ${source_dir}/src/GameEngine/*.cpp 
    ${source_dir}/src/NES/*.cpp)

#NANE executable
add_executable(NANE ${SOURCES})
target_link_libraries(NANE ${SDL2_LIBRARIES})
target_link_libraries(NANE ${SDL2_TTF_LIBRARIES})

set(CMAKE_EXECUTABLE_SUFFIX ".html")

target_compile_options(NANE PRIVATE 
    "-sUSE_SDL=2"
    "-sUSE_SDL_TTF=2"
)

target_link_options(NANE PRIVATE 
    "-sUSE_SDL=2"
    "-sUSE_SDL_TTF=2"
#    "--preload-file" "${source_dir}/resources/VeraMono.ttf@/VeraMono.ttf"
    "--preload-file" "${source_dir}/resources/@/"
)

#copy resource folder to target dirrectory
#add_custom_command(TARGET NANE PRE_BUILD
#                    COMMAND ${CMAKE_COMMAND} -E copy_directory
#                       ${source_dir}/resources/ $<TARGET_FILE_DIR:NANE>)