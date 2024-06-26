#ifndef MD_2024_POLYGONRC_H
#define MD_2024_POLYGONRC_H

#include "atlas/units/Rect.h"
#include "atlas/units/Brush.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"

class PolygonRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f uv;
    };

private:
    Shader *brushShader = nullptr;
    Brush brush;

    unsigned pointNum{0};

public:
    PolygonRC() = default;

    ~PolygonRC() override;

    void setData(const std::vector<Point>& points, const Brush &br);

    void setShaderData();

    void init(DC *dc) override;

    void render() override;
};

#endif //MD_2024_POLYGONRC_H
