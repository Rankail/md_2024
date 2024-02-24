#include "atlas/render/command/PolygonRC.h"

#include "atlas/polygon.h"
#include "atlas/render/DC.h"
#include "atlas/render/buffer/DynamicVertexBuffer.h"

PolygonRC::~PolygonRC() {
    delete vao;
}

void PolygonRC::init(DC* dc) {
    auto vbo = new DynamicVertexBuffer(sizeof(VertexData) * 3);

    vao = new VertexArray(vbo, nullptr, BufferLayout{
        ShaderDataType::Float2, ShaderDataType::Float2
    });

    brushShader = dc->getShader("brush");
}

void PolygonRC::setData(const std::vector<Point>& points, const Brush& br) {
    if (points.size() < 3) {
        pointNum = 0;
        return;
    }

    std::vector<Vec2f> pointVecs{};
    for (const auto& p : points) {
        pointVecs.emplace_back(p.x, p.y);
    }

    std::vector<Vec2f> triangles = triangulatePolygon(pointVecs);

    Vec2f topLeft = {points[0].x, points[0].y};
    Vec2f bottomRight{points[0].x, points[0].y};
    std::vector<VertexData> vertices{};
    for (const auto& vec : triangles) {
        topLeft.x() = std::min(topLeft.x(), vec.x());
        topLeft.y() = std::min(topLeft.y(), vec.y());
        bottomRight.x() = std::max(bottomRight.x(), vec.x());
        bottomRight.y() = std::max(bottomRight.y(), vec.y());
        vertices.emplace_back(vec);
    }
    const auto boundingSize = bottomRight - topLeft;

    for (auto& vertex : vertices) {
        vertex.uv = (vertex.pos - topLeft) / boundingSize;
    }

    vao->getVertexBuffer()->setData(vertices.data(), sizeof(VertexData) * vertices.size());

    this->brush = br;
    this->pointNum = triangles.size();
}

void PolygonRC::setShaderData() {
    brushShader->setInt("uBrushType", (int)brush.type);
    brushShader->setVector("uStartPos", brush.positionStart);
    brushShader->setVector("uEndPos", brush.positionEnd);
    brushShader->setVector("uAuxPos", brush.positionAux);
    brushShader->setColor("uStartColor", brush.colorStart);
    brushShader->setColor("uEndColor", brush.colorEnd);
    brushShader->setFloat("uBias", brush.bias);

    brushShader->setMatrix("uProjection", additionalData.transform);
}

void PolygonRC::render() {
    if (pointNum < 3) return;
    vao->bind();
    brushShader->use();
    setShaderData();
    glDrawArrays(GL_TRIANGLES, 0, pointNum);
}
