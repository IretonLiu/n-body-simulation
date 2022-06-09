#pragma once

#include <GL/glew.h>
#include <signal.h>

#include "index_buffer.h"
#include "shader.h"
#include "vertex_buffer.h"

// error handling
#define ASSERT(x) \
    if (!(x)) raise(SIGTRAP)

#define GLCall(x)   \
    GLClearError(); \
    x;              \
    ASSERT(GLLogCall(#x, __FILE__, __LINE__))

void GLClearError();
bool GLLogCall(const char* function, const char* file, int line);

// the main renderer class
class Renderer {
   public:
    // void Draw(const VertexBuffer& va, const IndexBuffer& ib, const Shader& shader) const;
    Renderer();
};
