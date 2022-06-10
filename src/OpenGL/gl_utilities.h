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

#include "../utilities.h"
#include "camera.h"
#include "geometry.h"
#include "index_buffer.h"
#include "renderer.h"
#include "shader.h"
#include "vertex_array.h"
#include "vertex_buffer.h"

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768

extern GLFWwindow* window;

int initGLProgram(const char* programName);

std::vector<float> GenVerticesFromBodies(std::vector<Body*> bodies, int P);

void render(std::vector<Body*> bodies, int P, void (*callback)(std::vector<Body*>&));
