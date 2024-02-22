#include "CircleRC.h"

#include "atlas/render/DC.h"

CircleRC::~CircleRC() {
    delete vao;
}

void CircleRC::init(DC *dc) {
    constexpr unsigned vertexCount = 4;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    unsigned indices[] = {0, 1, 3, 1, 2, 3};

    auto ebo = new ElementBuffer(sizeof(indices), indices);

    vao = new VertexArray(vbo, ebo, BufferLayout{
        ShaderDataType::Float2, ShaderDataType::Float2
    });

    circleShader = dc->getShader("circle");
}

void CircleRC::setData(Vec2d center, double radius, const Brush& br) {
    Vec2f point1{center.x() - radius, center.y() - radius};
    Vec2f point2{center.x() + radius, center.y() + radius};

    VertexData vertex[4] = {
        VertexData({point1.x(), point1.y()}, {0.f, 0.f}),
        VertexData({point2.x(), point1.y()}, {1.f, 0.f}),
        VertexData({point2.x(), point2.y()}, {1.f, 1.f}),
        VertexData({point1.x(), point2.y()}, {0.f, 1.f})
    };

    this->vao->getVertexBuffer()->setData(vertex, sizeof(vertex));
    this->brush = br;
}

void CircleRC::setShaderData() {
    circleShader->setInt("uBrushType", (int)brush.type);
    circleShader->setVector("uStartPos", brush.positionStart);
    circleShader->setVector("uEndPos", brush.positionEnd);
    circleShader->setVector("uAuxPos", brush.positionAux);
    circleShader->setColor("uStartColor", brush.colorStart);
    circleShader->setColor("uEndColor", brush.colorEnd);
    circleShader->setFloat("uBias", brush.bias);

    circleShader->setMatrix("uProjection", additionalData.transform);
}

void CircleRC::render() {
    vao->bind();
    circleShader->use();
    setShaderData();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
