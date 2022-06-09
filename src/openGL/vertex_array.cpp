
#include "vertex_array.h"

#include "renderer.h"

VertexArray::VertexArray() : attribEnabled(0) {
    GLCall(glGenVertexArrays(1, &rendererID));
    GLCall(glBindVertexArray(rendererID));
}

VertexArray::~VertexArray() {
    GLCall(glDeleteVertexArrays(1, &rendererID));
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout layout) {
    Bind();
    vb.Bind();
    const auto& attributes = layout.GetAttributes();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < attributes.size(); i++) {
        const auto& attribute = attributes[i];
        GLCall(glEnableVertexAttribArray(i + attribEnabled));
        GLCall(glVertexAttribPointer(
            i + attribEnabled,
            attribute.count,
            attribute.type,
            attribute.normalized,
            layout.GetStride(),
            (const void*)offset));
        offset += attribute.count * VertexAttribute::GetTypeSize(attribute.type);
    }
    attribEnabled += attributes.size();
}

void VertexArray::AddBuffer(const VertexBuffer& vb, const VertexBufferLayout layout, unsigned int layoutPosition) {
    Bind();
    vb.Bind();
    const auto& attributes = layout.GetAttributes();
    unsigned int offset = 0;
    for (unsigned int i = 0; i < attributes.size(); i++) {
        const auto& attribute = attributes[i];
        GLCall(glEnableVertexAttribArray(i + layoutPosition));
        GLCall(glVertexAttribPointer(
            i + layoutPosition,
            attribute.count,
            attribute.type,
            attribute.normalized,
            layout.GetStride(),
            (const void*)offset));
        offset += attribute.count * VertexAttribute::GetTypeSize(attribute.type);
    }
}

void VertexArray::Bind() const {
    GLCall(glBindVertexArray(rendererID));
}

void VertexArray::Unbind() const {
    GLCall(glBindVertexArray(0));
}
