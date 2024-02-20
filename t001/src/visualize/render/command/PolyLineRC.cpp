#include "PolyLineRC.h"

#include "visualize/render/DC.h"
#include "visualize/render/buffer/DynamicVertexBuffer.h"

PolyLineRC::~PolyLineRC() {
    delete vao;
}

void PolyLineRC::init(DC* dc) {
    auto vbo = new DynamicVertexBuffer(sizeof(VertexData) * 2);

    vao = new VertexArray(vbo, nullptr, BufferLayout{
        ShaderDataType::Float2, ShaderDataType::Float2
    });

    polyLineShader = dc->getShader("polyline");
}

void PolyLineRC::setData(const std::vector<Point>& points, const Brush& br, int lineWidth) {
    if (points.size() < 2) {
        lineNum = 0;
        return;
    }

    this->topLeft = {points[0].x, points[0].y};
    Vec2f bottomRight{points[0].x, points[0].y};
    std::vector<VertexData> vertices{};
    for (const auto& point : points) {
        topLeft.x() = std::min(topLeft.x(), (float)point.x);
        topLeft.y() = std::min(topLeft.y(), (float)point.y);
        bottomRight.x() = std::max(bottomRight.x(), (float)point.x);
        bottomRight.y() = std::max(bottomRight.y(), (float)point.y);
        vertices.emplace_back(Vec2f{point.x, point.y});
    }
    this->boundingSize = bottomRight - topLeft;

    for (int i = 1; i < vertices.size()-1; ++i) {
        const auto prevDir = vertices[i].pos - vertices[i - 1].pos;
        const auto nextDir = vertices[i + 1].pos - vertices[i].pos;
        const auto angle = prevDir.angleTo(nextDir);
        Vec2f corner;
        if (std::abs(std::fmod(angle, std::numbers::pi*2)) < 0.00001 || std::abs(std::numbers::pi - angle) < 0.00001) {
            corner = prevDir.rotated2d(std::numbers::pi / 2.).normalized();
        } else {
            double innerAngle = angle;
            if (angle > std::numbers::pi) {
                innerAngle = std::numbers::pi * 2. - angle;
            }

            const auto length = 1.f / std::abs(cos(innerAngle / 2.f));
            corner = (nextDir.normalized() - prevDir.normalized()).normalized() * length;
            if (angle > std::numbers::pi) {
                corner = -corner;
            }
        }
        vertices[i].corner = corner;
    }

    vertices[0].corner = Mat2f::rotation2d(std::numbers::pi / 2.) * (vertices[1].pos - vertices[0].pos).normalized();
    vertices.back().corner = Mat2f::rotation2d(std::numbers::pi / 2.) * (vertices.back().pos - vertices[vertices.size()-2].pos).normalized();

    vao->getVertexBuffer()->setData(vertices.data(), sizeof(VertexData) * vertices.size());

    this->brush = br;
    this->lineWidth = lineWidth;
    this->lineNum = points.size();
}

void PolyLineRC::setShaderData() {
    polyLineShader->setInt("uBrushType", (int)brush.type);
    polyLineShader->setVector("uStartPos", brush.positionStart);
    polyLineShader->setVector("uEndPos", brush.positionEnd);
    polyLineShader->setVector("uAuxPos", brush.positionAux);
    polyLineShader->setColor("uStartColor", brush.colorStart);
    polyLineShader->setColor("uEndColor", brush.colorEnd);
    polyLineShader->setFloat("uBias", brush.bias);

    polyLineShader->setInt("uLineWidth", lineWidth);
    polyLineShader->setMatrix("uProjection", additionalData.transform);
    polyLineShader->setVector("uTopLeft", topLeft);
    polyLineShader->setVector("uBoundingSize", boundingSize);
}

void PolyLineRC::render() {
    if (lineNum < 2) return;
    vao->bind();
    polyLineShader->use();
    setShaderData();
    glDrawArrays(GL_LINE_STRIP, 0, lineNum);
}
