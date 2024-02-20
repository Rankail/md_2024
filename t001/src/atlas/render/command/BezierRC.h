#ifndef MD_2024_BEZIERRC_H
#define MD_2024_BEZIERRC_H

#include "units/Rect.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"
#include "units/Brush.h"

class BezierRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()

public:
    struct VertexData {
        Vec2f pos;
    };

private:
    Shader* bezierShader;
    Brush brush;
    int lineWidth;

public:
    BezierRC() = default;
    virtual ~BezierRC();

    void setData(Point point1, Point point2, Point point3, Point point4, const Brush& br, int lineWidth = 1);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;

};

#endif //MD_2024_BEZIERRC_H
