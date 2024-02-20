#ifndef MD_2024_VERTEXARRAY_H
#define MD_2024_VERTEXARRAY_H

#include <vector>

#include "utils.h"
#include "visualize/render/buffer/ElementBuffer.h"
#include "visualize/render/buffer/VertexBuffer.h"

class VertexArray {
private:
    unsigned glID;
    unsigned attributeIndex = 0;
    VertexBuffer* vbo;
    ElementBuffer* ebo;

public:
    VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, const BufferLayout& layout);

    virtual ~VertexArray();

private:
    void makeAttribPointers(const BufferLayout& layout);

public:
    void bind() const;
    void unbind() const;

    VertexBuffer* getVertexBuffer() const { return vbo; }
    const ElementBuffer* getElementBuffer() const { return ebo; }
};

#endif //MD_2024_VERTEXARRAY_H
