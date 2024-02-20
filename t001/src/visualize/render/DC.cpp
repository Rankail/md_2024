#include "DC.h"

#include "visualize/render/command/LineRC.h"
#include "visualize/render/command/RectRC.h"
#include "visualize/render/command/TextureRC.h"
#include "visualize/render/command/TextRC.h"
#include "visualize/render/command/BezierRC.h"
#include "visualize/render/command/PolyLineRC.h"
#include "visualize/render/command/PolygonRC.h"

DC::DC()
    : commands(), width(0), height(0), commandCount(0) {
    registerRenderCommand<RectRC>();
    registerRenderCommand<PolygonRC>();
    registerRenderCommand<LineRC>();
    registerRenderCommand<PolyLineRC>();
    registerRenderCommand<BezierRC>();
    registerRenderCommand<TextureRC>();
    registerRenderCommand<TextRC>();
}

DC::~DC() {
    for (auto& rc : commands) delete rc;
}

void DC::init() {
    for (auto& rc : commands)
        rc->init(this);

    updateAdditionalData();
}

//====
// Shader functions
//====

bool DC::registerShader(const std::string& name, Shader *shader) {
    const auto [_, emplaced] = shaders.emplace(name, shader);
    return emplaced;
}

Shader *DC::getShader(const std::string& name) {
    auto it = shaders.find(name);
    return (it != shaders.end()) ? it->second : nullptr;
}

//====
// Texture funcs.
//====

bool DC::registerTexture(const std::string& name, Texture* texture) {
    auto it = textures.find(name);
    if (it != textures.end()) {
        if (texture != it->second) {
            delete it->second;
        }
        it->second = texture;
        return false;
    } else {
        textures.emplace(name, texture);
        return true;
    }
}

Texture *DC::getTexture(const std::string &name) {
    auto it = textures.find(name);
    return (it != textures.end()) ? it->second : nullptr;
}

bool DC::registerFontFamily(const std::string &name, FontFamily *fontFamily) {
    auto it = fontFamilies.find(name);
    if (it != fontFamilies.end()) {
        if (fontFamily != it->second) {
            delete it->second;
        }
        it->second = fontFamily;
        return false;
    } else {
        fontFamilies.emplace(name, fontFamily);
        return true;
    }
}

FontFamily *DC::getFontFamily(const std::string &name) {
    auto it = fontFamilies.find(name);
    return (it != fontFamilies.end()) ? it->second : nullptr;
}

//====
// Size funcs.
//====

void DC::setSize(unsigned width, unsigned height) {
    this->width = width;
    this->height = height;

    updateAdditionalData();
}

unsigned DC::getWidth() const {
    return width;
}

unsigned DC::getHeight() const {
    return height;
}

void DC::updateAdditionalData() {
    for (auto& rc : commands) {
        RC::AdditionalData data = {
            .transform = getTransformation().toFloat()
        };
        rc->setAdditionalData(data);
    }
}

//====
// Predefined drawing funcs.
//====

void DC::drawRect(Rect rect, const Brush& brush) {
    drawCustom<RectRC>(rect, brush);
}

void DC::drawLine(Point p1, Point p2, const Brush &br, int lineWidth) {
    drawCustom<LineRC>(p1, p2, br, lineWidth);
}

void DC::drawTexture(Rect rect, const std::string &textureName) {
    auto tex = getTexture(textureName);
    drawCustom<TextureRC>(rect, tex);
}

void DC::drawTexture(Rect rect, const Brush& br, const std::string &textureName) {
    auto tex = getTexture(textureName);
    drawCustom<TextureRC>(rect, br, tex);
}

void DC::drawText(Point pos, const std::string &text, const std::string &fontFamily, unsigned fontSize, const Brush &br, unsigned wrapLength) {
    auto *font = getFontFamily(fontFamily)->getFont(fontSize);
    drawCustom<TextRC>(pos, text, font, br, wrapLength);
}

void DC::drawText(Point pos, const std::string &text, const std::string &fontFamily, unsigned fontSize, unsigned wrapLength) {
    auto *font = getFontFamily(fontFamily)->getFont(fontSize);
    drawCustom<TextRC>(pos, text, font, Brush::solid(Colors::WHITE), wrapLength);
}

void DC::drawText(Point pos, const std::string &text, const std::string &fontFamily, FontStyle style, unsigned fontSize, const Brush &br, unsigned wrapLength) {
    auto *font = getFontFamily(fontFamily)->getFont(style, fontSize);
    drawCustom<TextRC>(pos, text, font, br, wrapLength);
}

void DC::drawText(Point pos, const std::string &text, const std::string &fontFamily, FontStyle style, unsigned fontSize, unsigned wrapLength) {
    auto *font = getFontFamily(fontFamily)->getFont(style, fontSize);
    drawCustom<TextRC>(pos, text, font, Brush::solid(Colors::WHITE), wrapLength);
}

void DC::drawText(Point pos, const std::string &text, Font *font, const Brush &br, unsigned wrapLength) {
    drawCustom<TextRC>(pos, text, font, br, wrapLength);
}

void DC::drawBezier(Point p1, Point p2, Point p3, Point p4, const Brush &br, int lineWidth) {
    drawCustom<BezierRC>(p1, p2, p3, p4, br, lineWidth);
}

void DC::drawPolyLine(const std::vector<Point> &points, const Brush &br, int lineWidth) {
    drawCustom<PolyLineRC>(points, br, lineWidth);
}

void DC::drawPolygon(const std::vector<Point> &points, const Brush &br) {
    drawCustom<PolygonRC>(points, br);
}


