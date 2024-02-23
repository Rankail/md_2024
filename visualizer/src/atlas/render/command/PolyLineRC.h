#ifndef MD_2024_POLYLINERC_H
#define MD_2024_POLYLINERC_H

#include "units/Rect.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"
#include "units/Brush.h"

class PolyLineRC : public RC {
RENDER_COMMAND()
RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f corner{0.f, 0.f};
    };

private:
    Shader *polyLineShader = nullptr;
    Brush brush;

    int lineWidth{0};
    unsigned lineNum{0};
    Vec2f topLeft;
    Vec2f boundingSize;

public:
    PolyLineRC() = default;

    ~PolyLineRC() override;

    void setData(const std::vector<Point>& points, const Brush &br, int lineWidth = 1);

    void setShaderData();

    void init(DC *dc) override;

    void render() override;

};

#endif //MD_2024_POLYLINERC_H
