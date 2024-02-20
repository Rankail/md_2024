#include "ElementBuffer.h"

#include <glad/glad.h>

ElementBuffer::ElementBuffer(unsigned count, unsigned *indices)
    : count(count) {
    glCreateBuffers(1, &glID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, indices, GL_STATIC_DRAW);
}

ElementBuffer::~ElementBuffer() {
    glDeleteBuffers(1, &glID);
}

void ElementBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glID);
}

void ElementBuffer::unbind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
