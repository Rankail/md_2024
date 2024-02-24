#include "atlas/render/command/RectRC.h"

#include "glad/glad.h"

#include "atlas/render/DC.h"

RectRC::~RectRC() {
    delete vao;
}

void RectRC::init(DC* dc) {
    constexpr unsigned vertexCount = 4;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    unsigned indices[] = {0, 1, 3, 1, 2, 3};

    auto ebo = new ElementBuffer(sizeof(indices), indices);

    vao = new VertexArray(vbo, ebo, BufferLayout{
            ShaderDataType::Float2, ShaderDataType::Float2
    });

    brushShader = dc->getShader("brush");
}

void RectRC::setData(Rect rect, const Brush &br) {
    Vec2f point1{rect.x, rect.y};
    Vec2f point2{rect.x + narrow(rect.dx), rect.y + narrow(rect.dy)};

    VertexData vertex[4] = {
        VertexData({point1.x(), point1.y()}, {0.f, 0.f}),
        VertexData({point2.x(), point1.y()}, {1.f, 0.f}),
        VertexData({point2.x(), point2.y()}, {1.f, 1.f}),
        VertexData({point1.x(), point2.y()}, {0.f, 1.f})
    };

    this->vao->getVertexBuffer()->setData(vertex, sizeof(vertex));
    this->brush = br;
}

void RectRC::setShaderData() {
    brushShader->setInt("uBrushType", (int)brush.type);
    brushShader->setVector("uStartPos", brush.positionStart);
    brushShader->setVector("uEndPos", brush.positionEnd);
    brushShader->setVector("uAuxPos", brush.positionAux);
    brushShader->setColor("uStartColor", brush.colorStart);
    brushShader->setColor("uEndColor", brush.colorEnd);
    brushShader->setFloat("uBias", brush.bias);

    brushShader->setMatrix("uProjection", additionalData.transform);
}

void RectRC::render() {
    vao->bind();
    brushShader->use();
    setShaderData();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
