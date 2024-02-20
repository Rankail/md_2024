#ifndef MD_2024_DC_H
#define MD_2024_DC_H

#include <queue>

#include "units/Rect.h"
#include "units/Brush.h"
#include "atlas/render/buffer/VertexBuffer.h"
#include "atlas/render/buffer/ElementBuffer.h"
#include "atlas/render/buffer/VertexArray.h"
#include "atlas/render/RC.h"
#include "atlas/render/Texture.h"
#include "atlas/render/Shader.h"
#include "atlas/render/font/Font.h"
#include "atlas/render/font/FontFamily.h"

class DC {
private:
    unsigned commandCount;

    std::vector<RC*> commands;

    std::unordered_map<std::string, Shader*> shaders;
    std::unordered_map<std::string, Texture*> textures;
    std::unordered_map<std::string, FontFamily*> fontFamilies;

    unsigned width;
    unsigned height;

public:
    explicit DC();
    ~DC();

    void init();

    template<typename T> requires IsRenderCommand<T>
    unsigned registerRenderCommand() {
        T::setIdx(commandCount);

        commands.emplace_back(new T());

        return commandCount++;
    }

    bool registerShader(const std::string& name, Shader* shader);
    Shader* getShader(const std::string& name);

    bool registerTexture(const std::string& name, Texture* texture);
    Texture* getTexture(const std::string& name);

    bool registerFontFamily(const std::string& name, FontFamily* fontFamily);
    FontFamily* getFontFamily(const std::string& name);

    void setSize(unsigned width, unsigned height);
    unsigned getWidth() const;
    unsigned getHeight() const;

    Mat4d getTransformation() const { return Mat4d::ortho(0, width, height, 0); }

private:
    void updateAdditionalData();

public:
    template<typename T, typename... Args> requires (IsRenderCommand<T> && HasRenderCommand<T, Args...>)
    void drawCustom(Args... args) {
        auto cmd = static_cast<T*>(commands[T::getStaticIdx()]);
        cmd->setData(args...);
        cmd->render();
    }

    void drawRect(Rect rect, const Brush& brush);

    void drawLine(Point p1, Point p2, const Brush &br, int lineWidth);
    void drawPolyLine(const std::vector<Point>& points, const Brush& br, int lineWidth);
    void drawBezier(Point p1, Point p2, Point p3, Point p4, const Brush &br, int lineWidth);

    void drawPolygon(const std::vector<Point>& points, const Brush& br);

    void drawTexture(Rect rect, const std::string& textureName);
    void drawTexture(Rect rect, const Brush &br, const std::string& textureName);

    void drawText(Point pos, const std::string& text, const std::string& fontFamily, FontStyle style, unsigned fontSize, const Brush& brush, unsigned wrapLength = 0);
    void drawText(Point pos, const std::string& text, const std::string& fontFamily, FontStyle style, unsigned fontSize, unsigned wrapLength = 0);
    void drawText(Point pos, const std::string& text, const std::string& fontFamily, unsigned fontSize, const Brush& brush, unsigned wrapLength = 0);
    void drawText(Point pos, const std::string& text, const std::string& fontFamily, unsigned fontSize, unsigned wrapLength = 0);
    void drawText(Point pos, const std::string& text, Font* font, const Brush& col, unsigned wrapLength = 0);

    void drawCircle(Point center, double radius, const Brush& brush);
};

#endif
