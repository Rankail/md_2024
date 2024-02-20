#include "BezierRC.h"

#include "visualize/render/DC.h"

BezierRC::~BezierRC() {

}

void BezierRC::setData(Point point1, Point point2, Point point3, Point point4, const Brush& br, int lineWidth) {
    VertexData vertex[4] = {
        VertexData({point1.x, point1.y}),
        VertexData({point2.x, point2.y}),
        VertexData({point3.x, point3.y}),
        VertexData({point4.x, point4.y})
    };

    vao->getVertexBuffer()->setData(vertex, sizeof(vertex));

    this->brush = br;
    this->lineWidth = lineWidth;
}

void BezierRC::setShaderData() {
    bezierShader->setInt("uBrushType", (int)brush.type);
    bezierShader->setVector("uStartPos", brush.positionStart);
    bezierShader->setVector("uEndPos", brush.positionEnd);
    bezierShader->setVector("uAuxPos", brush.positionAux);
    bezierShader->setColor("uStartColor", brush.colorStart);
    bezierShader->setColor("uEndColor", brush.colorEnd);
    bezierShader->setFloat("uBias", brush.bias);

    bezierShader->setInt("uLineWidth", lineWidth);
    bezierShader->setMatrix("uProjection", additionalData.transform);
}

void BezierRC::init(DC *dc) {
    constexpr unsigned vertexCount = 4;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    vao = new VertexArray(vbo, nullptr, BufferLayout{
        ShaderDataType::Float2
    });

    bezierShader = dc->getShader("bezier");
}

void BezierRC::render() {
    vao->bind();
    bezierShader->use();
    setShaderData();
    glDrawArrays(GL_LINES_ADJACENCY, 0, 4);
}
