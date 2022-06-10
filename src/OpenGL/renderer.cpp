#include "renderer.h"

#include <iostream>

void GLClearError() {
    while (glGetError() != GL_NO_ERROR)
        ;
}

bool GLLogCall(const char* function, const char* file, int line) {
    while (GLenum errCode = glGetError()) {
        std::cout << "[OpenGL Error] Error code: " << errCode << ", "
                  << "Error: " << gluErrorString(errCode) << " "
                  << "at " << file << ":" << line << std::endl;
        return false;
    }
    return true;
}
