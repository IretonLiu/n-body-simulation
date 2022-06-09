#pragma once

class IndexBuffer {
   private:
    unsigned int rendererID;  // object id in renderer
    unsigned int count;

   public:
    IndexBuffer(const unsigned int* data, unsigned int count);
    ~IndexBuffer();

    void Bind() const;
    void Unbind() const;
};