#ifndef GLHelper_HPP
#define GLHelper_HPP

// openGL libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// error library
#include <GL/freeglut.h>
#include <GL/glu.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard c++ libraries
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <filesystem>
#include <fstream>
#include <glm/glm.hpp>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
// custom includes
#include "camera.h"
#include "geometry.h"
#include "index_buffer.h"
#include "renderer.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"

extern GLFWwindow* window;

int initGLProgram(const char* programName);
// int loadShaders(const char* vertexshader_path, const char* fragmentshader_path);
void render(PerspectiveCamera* camera, Geometry* geometry);
// void createVBOs(Geometry* geometry);

#endif