#ifndef MD_2024_TEXTRC_H
#define MD_2024_TEXTRC_H

#include "atlas/units/Rect.h"
#include "atlas/units/Brush.h"
#include "atlas/render/RC.h"
#include "atlas/render/Shader.h"
#include "atlas/render/Texture.h"
#include "atlas/render/font/Font.h"

#define TEXT_CACHE_SIZE 100

struct TextData {
    SDL_Surface* surface;
    Font* font;
    std::string text;
    unsigned glID;
};

class TextRC : public RC {
    RENDER_COMMAND()
    RENDER_COMMAND_BUFFERS()
public:
    struct VertexData {
        Vec2f pos;
        Vec2f texCoord;
    };

private:
    Shader*   textureShader = nullptr;
    TextData* currentText = nullptr;
    TextData  textCache[TEXT_CACHE_SIZE];

    unsigned      cacheIdx{0};
    unsigned      wrapLength{0};
    Brush     brush;

private:
    SDL_Surface* createTextSurface(const std::string& text, Font* font) const;

    void bindTexture();

public:
    TextRC() = default;
    ~TextRC() override;

    void setData(Point pos, const std::string& text, Font* font, const Brush& br, unsigned wrapLength = 0);
    void setShaderData();

    void init(DC* dc) override;
    void render() override;
};

#endif //MD_2024_TEXTRC_H
