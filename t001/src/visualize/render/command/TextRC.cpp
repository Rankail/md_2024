#include "TextRC.h"

#include "visualize/render/DC.h"
#include "Logger.h"

#define SDL_WHITE {0xFF, 0xFF, 0xFF, 0xFF}
#define SDL_BLACK {0, 0, 0, 0xFF}

TextRC::~TextRC() {
    delete vao;
    for (auto& textData : textCache) {
        if (textData.surface != nullptr) {
            SDL_FreeSurface(textData.surface);
            if (textData.glID != 0) {
                glDeleteTextures(1, &(textData.glID));
            }
        }
    }
}

void TextRC::init(DC *dc) {
    constexpr unsigned vertexCount = 4;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    unsigned indices[] = {0, 1, 3, 1, 2, 3};

    auto ebo = new ElementBuffer(sizeof(indices), indices);

    vao = new VertexArray(vbo, ebo, BufferLayout{
        ShaderDataType::Float2, ShaderDataType::Float2
    });

    textureShader = dc->getShader("text");
    cacheIdx = 0;
    for (auto & i : textCache) {
        i = {nullptr, nullptr, "", 0};
    }
}

SDL_Surface *TextRC::createTextSurface(const std::string &text, Font *font) const {
    SDL_Surface* surface;

    surface = TTF_RenderUTF8_Shaded_Wrapped(font->getFont(), text.c_str(), SDL_WHITE, SDL_BLACK, wrapLength);
    if (surface == nullptr) {
        TET_CRITICAL("Failed to create Text '", text, "'");
    }

    // Wtf. Color formats in SDL are just strange. Why is this blit needed?
    // There seems to be some kind of 'padding' in the original texture that is blitted
    // out here. Silly SDL.
    SDL_Surface* s = SDL_CreateRGBSurface(0, surface->w, surface->h, 32, 0x00ff0000,0x0000ff00,0x000000ff,0xff000000);
    SDL_BlitSurface(surface, nullptr, s, nullptr);
    SDL_FreeSurface(surface);
    return s;
}

void TextRC::setData(Point pos, const std::string& text, Font* font, const Brush& br, unsigned wrapLength) {
    this->brush = br;
    this->wrapLength = wrapLength;

    // check cache
    currentText = nullptr;
    for (auto & i : textCache) {
        if (i.font == font && i.text == text) {
            currentText = &i;
            break;
        }
    }

    // not in cache
    if (currentText == nullptr) {
        SDL_Surface* surface = createTextSurface(text, font);

        currentText = &textCache[cacheIdx];

        if (currentText->surface != nullptr) {
            SDL_FreeSurface(currentText->surface);
            if (currentText->glID != 0) {
                glBindTexture(GL_TEXTURE_2D, 0);
                glDeleteTextures(1, &currentText->glID);
            }
        }

        textCache[cacheIdx] = TextData{surface, font, text, 0};
        cacheIdx = (cacheIdx + 1) % TEXT_CACHE_SIZE;
    }

    Vec2f point1{pos.x, pos.y};
    Vec2f point2{pos.x + narrow(currentText->surface->w), pos.y + narrow(currentText->surface->h)};

    VertexData vertex[4] = {
        VertexData({point1.x(), point1.y()}, {0.f, 0.f}),
        VertexData({point2.x(), point1.y()}, {1.f, 0.f}),
        VertexData({point2.x(), point2.y()}, {1.f, 1.f}),
        VertexData({point1.x(), point2.y()}, {0.f, 1.f})
    };

    vao->getVertexBuffer()->setData(vertex, sizeof(vertex));
}

void TextRC::setShaderData() {
    textureShader->setInt("uBrushType", (int)brush.type);
    textureShader->setVector("uStartPos", brush.positionStart);
    textureShader->setVector("uEndPos", brush.positionEnd);
    textureShader->setVector("uAuxPos", brush.positionAux);
    textureShader->setColor("uStartColor", brush.colorStart);
    textureShader->setColor("uEndColor", brush.colorEnd);
    textureShader->setFloat("uBias", brush.bias);

    textureShader->setMatrix("uProjection", additionalData.transform);
}

void TextRC::render() {
    vao->bind();
    textureShader->use();

    setShaderData();
    bindTexture();

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}

void TextRC::bindTexture() {
    if (currentText->glID == 0) {
        SDL_Surface* surface = currentText->surface;
        if (surface == nullptr) {
            TET_CRITICAL("Text was not loaded");
        }

        glGenTextures(1, &currentText->glID);
        glBindTexture(GL_TEXTURE_2D, currentText->glID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int channels = surface->format->BytesPerPixel;
        glTexImage2D(GL_TEXTURE_2D, 0, channels, surface->w, surface->h, 0, GL_BGRA, GL_UNSIGNED_INT_8_8_8_8_REV, surface->pixels);
    }

    glBindTexture(GL_TEXTURE_2D, currentText->glID);
}
