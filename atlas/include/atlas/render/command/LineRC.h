#ifndef MD_2024_LINERC_H
#define MD_2024_LINERC_H

#include "atlas/units/Rect.h"
#include "atlas/units/Brush.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"

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

    void setData(Vec2f point1, Vec2f point2, const Brush& br, int lineWidth = 1);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;
};

#endif //MD_2024_LINERC_H
