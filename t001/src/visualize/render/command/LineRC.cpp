#include "LineRC.h"

#include "visualize/render/DC.h"

LineRC::~LineRC() {
    delete vao;
}

void LineRC::init(DC* dc) {
    constexpr unsigned vertexCount = 2;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    vao = new VertexArray(vbo, nullptr, BufferLayout{
        ShaderDataType::Float2
    });

    lineShader = dc->getShader("line");
}

void LineRC::setData(Point point1, Point point2, const Brush& br, int lineWidth) {
    VertexData vertex[2] = {
        VertexData({point1.x, point1.y}),
        VertexData({point2.x, point2.y})
    };
    vao->getVertexBuffer()->setData(vertex, sizeof(vertex));

    this->brush = br;
    this->lineWidth = lineWidth;
}

void LineRC::setShaderData() {
    lineShader->setInt("uBrushType", (int)brush.type);
    lineShader->setVector("uStartPos", brush.positionStart);
    lineShader->setVector("uEndPos", brush.positionEnd);
    lineShader->setVector("uAuxPos", brush.positionAux);
    lineShader->setColor("uStartColor", brush.colorStart);
    lineShader->setColor("uEndColor", brush.colorEnd);
    lineShader->setFloat("uBias", brush.bias);

    lineShader->setInt("uLineWidth", lineWidth);
    lineShader->setMatrix("uProjection", additionalData.transform);
}

void LineRC::render() {
    vao->bind();
    lineShader->use();
    setShaderData();
    glDrawArrays(GL_LINE_STRIP, 0, 2);
}
