#include "atlas/render/buffer/VertexArray.h"

#include "glad/glad.h"

static GLenum openGLTypeFromShaderDataType(ShaderDataType type) {
    switch (type) {
        case ShaderDataType::Float:
        case ShaderDataType::Float2:
        case ShaderDataType::Float3:
        case ShaderDataType::Float4:
            return GL_FLOAT;
        case ShaderDataType::Int:
        case ShaderDataType::Int2:
        case ShaderDataType::Int3:
        case ShaderDataType::Int4:
            return GL_INT;
        case ShaderDataType::Mat3:
        case ShaderDataType::Mat4:
            return GL_FLOAT;
        case ShaderDataType::Bool:
            return GL_BOOL;
        default:
            return 0;
    }
}

VertexArray::VertexArray(VertexBuffer* vbo, ElementBuffer* ebo, const BufferLayout& layout)
    : vbo(vbo), ebo(ebo) {
    glCreateVertexArrays(1, &glID);
    makeAttribPointers(layout);
}

VertexArray::~VertexArray() {
    glDeleteVertexArrays(1, &glID);
}

void VertexArray::makeAttribPointers(const BufferLayout &layout) {
    bind();
    vbo->bind();

    for (const auto& element: layout) {
        switch (element.type) {
            case ShaderDataType::Float:
            case ShaderDataType::Float2:
            case ShaderDataType::Float3:
            case ShaderDataType::Float4: {
                glEnableVertexAttribArray(attributeIndex);
                glVertexAttribPointer(
                    attributeIndex,
                    element.getComponentCount(),
                    openGLTypeFromShaderDataType(element.type),
                    element.normalized ? GL_TRUE : GL_FALSE,
                    layout.getStride(),
                    (const void *) element.offset
                                     );
                attributeIndex++;
                break;
            }
            case ShaderDataType::Int:
            case ShaderDataType::Int2:
            case ShaderDataType::Int3:
            case ShaderDataType::Int4:
            case ShaderDataType::Bool: {
                glEnableVertexAttribArray(attributeIndex);
                glVertexAttribIPointer(
                    attributeIndex,
                    element.getComponentCount(),
                    openGLTypeFromShaderDataType(element.type),
                    layout.getStride(),
                    (const void *) element.offset
                                      );
                attributeIndex++;
                break;
            }
            case ShaderDataType::Mat3:
            case ShaderDataType::Mat4: {
                uint8_t count = element.getComponentCount();
                for (uint8_t i = 0; i < count; i++) {
                    glEnableVertexAttribArray(attributeIndex);
                    glVertexAttribPointer(
                        attributeIndex,
                        count,
                        openGLTypeFromShaderDataType(element.type),
                        element.normalized ? GL_TRUE : GL_FALSE,
                        layout.getStride(),
                        (const void *) (element.offset + sizeof(float) * count * i)
                                         );
                    glVertexAttribDivisor(attributeIndex, 1);
                    attributeIndex++;
                }
                break;
            }
        }
    }
}

void VertexArray::bind() const {
    glBindVertexArray(glID);
    if (ebo != nullptr) {
        ebo->bind();
    }
}

void VertexArray::unbind() const {
    glBindVertexArray(0);
}
