cmake_minimum_required(VERSION 3.17)
project(NBodySim)

# set(GCC_COVERAGE_COMPILE_FLAGS "-Wall -g")
set(GCC_COVERAGE_COMPILE_FLAGS "-O4 -Wall -g -lGL -lglfw -lGLEW -lGLU")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${GCC_COVERAGE_COMPILE_FLAGS}" )
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

add_library(
	utils
	src/body.h
	src/body.cpp
	src/vec3.h
	src/vec3.cpp
	src/utilities.h
	src/utilities.cpp
	src/node.h
	src/node.cpp
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


# FILE(
# 	GLOB
# 	SRCS
# 	src/*.cpp)

add_executable(DataGenerator src/data_generator.cpp)
add_executable(Serial src/serial.cpp)

target_link_libraries(DataGenerator PRIVATE utils)
target_link_libraries(Serial PRIVATE utils gl_utils)

