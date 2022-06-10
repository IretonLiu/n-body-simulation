#pragma once

#include <vector>

#include "renderer.h"

struct VertexAttribute {
    unsigned int type;
    unsigned int count;
    unsigned char normalized;

    // return the size of the datatype specified
    static unsigned int GetTypeSize(unsigned int type) {
        switch (type) {
            case GL_FLOAT:
                return 4;
            case GL_UNSIGNED_INT:
                return 4;
            case GL_UNSIGNED_BYTE:  // 0 - 255, such as colour values
                return 1;
                // add more when needed
        }
        ASSERT(false);
        return 0;
    }
};

/*
 * class for the layout of the vertex buffer
 * used in the vertex array object
 * a vertex and have different layouts (position, colour, normal, ..., etc)
 */

class VertexBufferLayout {
   private:
    // use a vector because there can be multiply buffers
    // each buffer with a different attribute
    std::vector<VertexAttribute> attributes;
    unsigned int stride;

   public:
    VertexBufferLayout() : stride(0){};
    /* consider using templates for this Push function
     * so that arbitrary types can be implemented
     */
    inline void Push(unsigned int type, unsigned int count, unsigned char normalized) {
        attributes.push_back({type, count, normalized});
        stride += VertexAttribute::GetTypeSize(type) * count;
    }
    inline const std::vector<VertexAttribute> GetAttributes() const { return attributes; };
    inline const unsigned int GetStride() const { return stride; };
};