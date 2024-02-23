#include "BufferElement.h"

static unsigned getShaderDataTypeSize(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:     return 4;
        case ShaderDataType::Float2:    return 4 * 2;
        case ShaderDataType::Float3:    return 4 * 3;
        case ShaderDataType::Float4:    return 4 * 4;
        case ShaderDataType::Int:       return 4;
        case ShaderDataType::Int2:      return 4 * 2;
        case ShaderDataType::Int3:      return 4 * 3;
        case ShaderDataType::Int4:      return 4 * 4;
        case ShaderDataType::Mat3:      return 4 * 3 * 3;
        case ShaderDataType::Mat4:      return 4 * 4 * 4;
        case ShaderDataType::Bool:      return 1;
        default:                        return 0;
    }
}

BufferElement::BufferElement(ShaderDataType type, bool normalized)
    : type(type), size(getShaderDataTypeSize(type)), offset(0), normalized(normalized) {}

unsigned BufferElement::getComponentCount() const {
    switch (type) {
        case ShaderDataType::Float:     return 1;
        case ShaderDataType::Float2:    return 2;
        case ShaderDataType::Float3:    return 3;
        case ShaderDataType::Float4:    return 4;
        case ShaderDataType::Int:       return 1;
        case ShaderDataType::Int2:      return 2;
        case ShaderDataType::Int3:      return 3;
        case ShaderDataType::Int4:      return 4;
        case ShaderDataType::Mat3:      return 3;
        case ShaderDataType::Mat4:      return 4;
        case ShaderDataType::Bool:      return 1;
        default:                        return 0;
    }
}
