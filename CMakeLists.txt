cmake_minimum_required(VERSION 3.8)
project(NBodySim LANGUAGES CXX CUDA)

find_package(MPI)
# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -g")
set(GCC_COVERAGE_COMPILE_FLAGS " -g -lGL -lglfw -lGLEW -lGLU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
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

add_library(
	gl_utils
	src/OpenGL/camera.h
	src/OpenGL/camera.cpp
	src/OpenGL/geometry.h
	src/OpenGL/geometry.cpp
	src/OpenGL/gl_utilities.h
	src/OpenGL/gl_utilities.cpp
	src/OpenGL/index_buffer.h
	src/OpenGL/index_buffer.cpp
	src/OpenGL/renderer.h
	src/OpenGL/renderer.cpp
	src/OpenGL/shader.h
	src/OpenGL/shader.cpp
	src/OpenGL/vertex_array.h
	src/OpenGL/vertex_array.cpp
	src/OpenGL/vertex_buffer.h
	src/OpenGL/vertex_buffer.cpp
	src/OpenGL/vertex_buffer_layout.h
)

	
add_executable(DataGenerator src/data_generator.cpp)
add_executable(Serial src/serial.cpp)
add_executable(Cuda src/cuda.cu)
add_executable(MPI src/mpi.cpp)

target_link_libraries(DataGenerator PRIVATE utils)
target_link_libraries(Serial PRIVATE utils gl_utils)
target_link_libraries(Cuda PRIVATE utils gl_utils)
target_link_libraries(MPI PRIVATE utils gl_utils ${MPI_CXX_LIBRARIES})
target_compile_options(Cuda PRIVATE $<$<COMPILE_LANGUAGE:CUDA>:
												-g;
												-G;
												-arch=sm_61;
                        -std=c++11;
>)
