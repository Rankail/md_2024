#ifndef MD_2024_RECTRC_H
#define MD_2024_RECTRC_H

#include "units/Rect.h"
#include "units/Color.h"
#include "units/Brush.h"
#include "vec.h"
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
