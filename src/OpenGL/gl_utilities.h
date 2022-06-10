#pragma once

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

#include "geometry.h"
#include "index_buffer.h"
#include "renderer.h"
#include "shader.h"
#include "utilities.h"
#include "vertex_array.h"
#include "vertex_buffer.h"

#define WINDOW_WIDTH 256
#define WINDOW_HEIGHT 256

extern GLFWwindow* window;

extern float canvas[WINDOW_WIDTH * WINDOW_HEIGHT];

int initGLProgram(const char* programName);

void ClearCanvas();

void UpdateCanvas(std::vector<Body*> bodies, int P);