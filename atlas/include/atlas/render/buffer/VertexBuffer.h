#ifndef MD_2024_VERTEXBUFFER_H
#define MD_2024_VERTEXBUFFER_H

#include "utils/utils.h"
#include "BufferLayout.h"

class VertexBuffer {
protected:
    unsigned glID;

public:
    /**
     * Creates an empty vertexbuffer
     * @param size - in bytes
     */
    VertexBuffer(unsigned size);

    /**
     * Creates a vertexbuffer
     * @param size - in bytes
     * @param vertices - data
     */
    VertexBuffer(unsigned size, void* vertices);

    VertexBuffer(const VertexBuffer& o) = delete;

    virtual ~VertexBuffer();


    virtual void setData(const void *data, unsigned size);

    void bind() const;
    void unbind() const;
};


#endif //MD_2024_VERTEXBUFFER_H
