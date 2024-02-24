#ifndef MD_2024_BUFFERELEMENT_H
#define MD_2024_BUFFERELEMENT_H

#include <string>

#include "utils/utils.h"

enum class ShaderDataType {
    None = 0,
    Float, Float2, Float3, Float4,
    Int, Int2, Int3, Int4,
    Mat3, Mat4,
    Bool
};

struct BufferElement {
    ShaderDataType type;
    unsigned size;
    size_t offset;
    bool normalized;

    BufferElement() = default;
    BufferElement(ShaderDataType type, bool normalized = false);

    unsigned getComponentCount() const;
};

#endif //MD_2024_BUFFERELEMENT_H
