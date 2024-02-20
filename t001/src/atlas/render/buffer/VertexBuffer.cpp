#include "VertexBuffer.h"

#include <glad/glad.h>

VertexBuffer::VertexBuffer(unsigned size) {
    glCreateBuffers(1, &glID);
    glBindBuffer(GL_ARRAY_BUFFER, glID);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
}

VertexBuffer::VertexBuffer(unsigned size, void* vertices) {
    glCreateBuffers(1, &glID);
    glBindBuffer(GL_ARRAY_BUFFER, glID);
    glBufferData(GL_ARRAY_BUFFER, size, vertices, GL_DYNAMIC_DRAW);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &glID);
}

void VertexBuffer::setData(const void *data, unsigned size) {
    glBindBuffer(GL_ARRAY_BUFFER, glID);
    glBufferSubData(GL_ARRAY_BUFFER, 0, size, data);
}

void VertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, glID);
}

void VertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
