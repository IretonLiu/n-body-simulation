#pragma once

#include "vertex_buffer.h"
#include "vertex_buffer_layout.h"

class VertexArray {
   private:
    unsigned int rendererID;
    unsigned int attribEnabled;  // number of attribute enabled, starting from 0

   public:
    VertexArray();
    ~VertexArray();

    /*
     * Adds a vertex buffer and its associate vertex layouts
     * @param vb the vertex buffer
     * @param layout the layout specification
     */
    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout layout);

    /*
     * Adds a vertex buffer and its associate vertex layouts and the vertex attribute array to enable
     * @param vb the vertex buffer
     * @param layout the layout specification
     * @param layoutPosition the attribute position to enable from
     */
    void AddBuffer(const VertexBuffer& vb, const VertexBufferLayout layout, unsigned int layoutPosition);
    void Bind() const;
    void Unbind() const;
};