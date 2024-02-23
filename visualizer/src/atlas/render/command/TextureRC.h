#ifndef MD_2024_TEXTURERC_H
#define MD_2024_TEXTURERC_H

#include "units/Rect.h"
#include "atlas/render/RC.h"
#include "atlas/render/Texture.h"
#include "atlas/render/Shader.h"
#include "units/Brush.h"

class TextureRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f texCoord;
    };

private:
    Shader*  textureShader = nullptr;
    Texture* texture = nullptr;

    bool     useBrush = false;
    Brush    brush;

public:
    TextureRC() = default;
    ~TextureRC() override;

    void setData(Rect rect, const Brush &br, Texture* tex);
    void setData(Rect rect, Texture* tex);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;
};

#endif //MD_2024_TEXTURERC_H
