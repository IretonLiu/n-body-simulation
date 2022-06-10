#pragma once

// the vertex buffer class
//
class VertexBuffer {
   private:
    unsigned int rendererID;  // object id in renderer
   public:
    VertexBuffer(const void* data, unsigned int size);
    ~VertexBuffer();

    void Bind() const;
    void Unbind() const;
};