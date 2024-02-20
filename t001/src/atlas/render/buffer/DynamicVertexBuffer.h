#ifndef MD_2024_DYNAMICVERTEXBUFFER_H
#define MD_2024_DYNAMICVERTEXBUFFER_H

#include "VertexBuffer.h"

class DynamicVertexBuffer : public VertexBuffer {
private:
    unsigned size;

public:
    DynamicVertexBuffer(unsigned size = 0);
    DynamicVertexBuffer(unsigned size, void* vertices);
    ~DynamicVertexBuffer() override = default;

    void setData(const void* data, unsigned size) override;
};

#endif //MD_2024_DYNAMICVERTEXBUFFER_H
