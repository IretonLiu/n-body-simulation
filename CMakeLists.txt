cmake_minimum_required(VERSION 3.8)
project(NBodySim LANGUAGES CXX CUDA)

find_package(MPI)
# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}" )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

add_library(
	utils
	src/utils/body.h
	src/utils/body.cpp
	src/utils/vec3.h
	src/utils/vec3.cpp
	src/utils/utilities.h
	src/utils/utilities.cpp
	src/utils/node.h
	src/utils/node.cpp
	src/utils/helper_cuda.h
	src/utils/helper_string.h
)
	
add_executable(DataGenerator src/data_generator.cpp)
add_executable(Serial src/serial.cpp)
add_executable(Cuda src/cuda.cu)
add_executable(MPI src/mpi.cpp)
add_executable(Validate src/validator.cpp)

target_link_libraries(DataGenerator PRIVATE utils)
target_link_libraries(Serial PRIVATE utils)
target_link_libraries(Cuda PRIVATE utils)
target_link_libraries(MPI PRIVATE utils ${MPI_CXX_LIBRARIES})
target_compile_options(Cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
												-g;
												-G;
												-arch=sm_61;
                        -std=c++11;
>)
