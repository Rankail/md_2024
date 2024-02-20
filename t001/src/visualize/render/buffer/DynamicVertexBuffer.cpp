#include "DynamicVertexBuffer.h"

#include <glad/glad.h>

DynamicVertexBuffer::DynamicVertexBuffer(unsigned size)
    : VertexBuffer(size), size(size) {}

DynamicVertexBuffer::DynamicVertexBuffer(unsigned size, void *vertices)
    : VertexBuffer(size, vertices), size(size) {}

void DynamicVertexBuffer::setData(const void *data, unsigned size) {
    if (this->size < size) {
        this->size = std::max(size, this->size * 2);
        glBindBuffer(GL_ARRAY_BUFFER, glID);
        glBufferData(GL_ARRAY_BUFFER, this->size, data, GL_DYNAMIC_DRAW);
    } else {
        VertexBuffer::setData(data, size);
    }
}
