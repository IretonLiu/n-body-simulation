cmake_minimum_required(VERSION 3.17)
project(NBodySim)

set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(
	utils
	src/utilities.h
	src/utilities.cpp
)

# FILE(
# 	GLOB
# 	SRCS
# 	src/*.cpp)

add_executable(DataGenerator src/data_generator.cpp)
add_executable(Serial src/serial.cpp)

target_link_libraries(DataGenerator PRIVATE utils)
target_link_libraries(Serial PRIVATE utils)

