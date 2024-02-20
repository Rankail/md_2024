#ifndef MD_2024_LINERC_H
#define MD_2024_LINERC_H

#include "units/Rect.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"
#include "units/Brush.h"

class LineRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
    };

private:
    Shader* lineShader = nullptr;
    Brush brush;

    int lineWidth;

public:
    LineRC() = default;
    ~LineRC() override;

    void setData(Point point1, Point point2, const Brush& br, int lineWidth = 1);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;
};

#endif //MD_2024_LINERC_H
