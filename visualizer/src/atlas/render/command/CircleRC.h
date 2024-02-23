#ifndef MD_2024_CIRCLERC_H
#define MD_2024_CIRCLERC_H


#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"
#include "units/Brush.h"
#include "units/Point.h"

class DC;

class CircleRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f uv;
    };

private:
    Shader* circleShader = nullptr;
    Brush brush;

public:
    CircleRC() = default;
    ~CircleRC() override;

    void setData(Vec2d center, double radius, const Brush& br);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;
};


#endif //MD_2024_CIRCLERC_H
