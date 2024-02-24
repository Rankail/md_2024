#include "atlas/render/command/TextureRC.h"

#include "atlas/render/DC.h"

TextureRC::~TextureRC() {
    delete vao;
}

void TextureRC::init(DC *dc) {
    constexpr unsigned vertexCount = 4;
    auto vbo = new VertexBuffer(sizeof(VertexData) * vertexCount);

    unsigned indices[] = {0, 1, 3, 1, 2, 3};

    auto ebo = new ElementBuffer(sizeof(indices), indices);

    vao = new VertexArray(vbo, ebo, BufferLayout{
        ShaderDataType::Float2, ShaderDataType::Float2
    });

    textureShader = dc->getShader("texture");
}

void TextureRC::setData(Rect rect, const Brush &br, Texture *tex)
{
    this->setData(rect, tex);

    this->brush    = br;
    this->useBrush = true;
}

void TextureRC::setData(Rect rect, Texture *tex) {
    Vec2f point1{rect.x, rect.y};
    Vec2f point2{rect.x + narrow(rect.dx), rect.y + narrow(rect.dy)};

    VertexData vertex[4] = {
        VertexData({point1.x(), point1.y()}, {0.f, 0.f}),
        VertexData({point2.x(), point1.y()}, {1.f, 0.f}),
        VertexData({point2.x(), point2.y()}, {1.f, 1.f}),
        VertexData({point1.x(), point2.y()}, {0.f, 1.f})
    };

    vao->getVertexBuffer()->setData(vertex, sizeof(vertex));

    this->texture = tex;
    this->useBrush = false;
}

void TextureRC::setShaderData() {
    textureShader->setMatrix("uProjection", additionalData.transform);

    if (!useBrush) {
        // Save a bit of performance by not creating
        // white brushes all the time.
        textureShader->setInt("uBrushType", 0xFF);
        return;
    }

    textureShader->setInt("uBrushType", (int)brush.type);
    textureShader->setVector("uStartPos", brush.positionStart);
    textureShader->setVector("uEndPos", brush.positionEnd);
    textureShader->setVector("uAuxPos", brush.positionAux);
    textureShader->setColor("uStartColor", brush.colorStart);
    textureShader->setColor("uEndColor", brush.colorEnd);
    textureShader->setFloat("uBias", brush.bias);
}

void TextureRC::render() {
    vao->bind();
    textureShader->use();
    setShaderData();
    texture->bind();
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
