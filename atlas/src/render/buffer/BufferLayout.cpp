#include "atlas/render/buffer/BufferLayout.h"

void BufferLayout::calcOffsetAndStride() {
    size_t offset = 0;
    stride = 0;
    for (auto& element : elements) {
        element.offset = offset;
        offset += element.size;
    }
    stride = offset;
}
