#ifndef MD_2024_RECTRC_H
#define MD_2024_RECTRC_H

#include "utils/math/vec.h"
#include "atlas/units/Rect.h"
#include "atlas/units/Color.h"
#include "atlas/units/Brush.h"
#include "atlas/render/RC.h"
#include "atlas/render/buffer/VertexArray.h"
#include "atlas/render/Shader.h"

class DC;

class RectRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f uv;
    };

private:
    Shader* brushShader = nullptr;
    Brush brush;

private:
    void setShaderData();

public:
    RectRC() = default;
    ~RectRC() override;

    void init(DC* dc) override;
    void setData(Rect rect, const Brush &br);
    void render() override;
};

#endif //MD_2024_RECTRC_H
