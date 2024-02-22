#include "Window.h"

#include <glad/glad.h>

#include "Logger.h"

Window::Window(const std::string &title, const Vec2u& size)
    : Window(title, Vec2i{-1, -1}, size, WindowFlag::SHOWN) {}

Window::Window(const std::string& title, Vec2i position, Vec2u size, WindowFlag flags) {
    if (position.x() < 0) {
        position.x() = SDL_WINDOWPOS_UNDEFINED;
    }
    if (position.y() < 0) {
        position.y() = SDL_WINDOWPOS_UNDEFINED;
    }

    handle = SDL_CreateWindow(title.c_str(), (int)position.x(), (int)position.y(),
        (int)size.x(), (int)size.y(), SDL_WINDOW_OPENGL | (unsigned)flags);

    if (handle == nullptr) {
        TET_ERROR("Failed to create window. {}", SDL_GetError());
        return;
    }

    context = SDL_GL_CreateContext(handle);
    if (context == nullptr) {
        TET_ERROR("Failed to create context. {}", SDL_GetError());
        return;
    }
}



Window::~Window() {
    SDL_GL_DeleteContext(context);
    SDL_DestroyWindow(handle);
}

void Window::close() {
    SDL_DestroyWindow(handle);
}

void Window::setCaption(const std::string &capt) {
    SDL_SetWindowTitle(handle, capt.c_str());
}

void Window::init() {
    int w, h;
    SDL_GetWindowSize(handle, &w, &h);

    glDisable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glViewport(0, 0, w, h);

    auto textureShader = new Shader();
    textureShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    textureShader->addSourceAndCompile("./rsc/shader/texture.frag", GL_FRAGMENT_SHADER);
    textureShader->link();

    auto textureMaskShader = new Shader();
    textureMaskShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    textureMaskShader->addSourceAndCompile("./rsc/shader/alphamask.frag", GL_FRAGMENT_SHADER);
    textureMaskShader->link();

    auto brushShader = new Shader();
    brushShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    brushShader->addSourceAndCompile("./rsc/shader/fill.frag", GL_FRAGMENT_SHADER);
    brushShader->link();

    auto nebulaShader = new Shader();
    nebulaShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    nebulaShader->addSourceAndCompile("./rsc/shader/nebula.frag", GL_FRAGMENT_SHADER);
    nebulaShader->link();

    auto innerglowShader = new Shader();
    innerglowShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    innerglowShader->addSourceAndCompile("./rsc/shader/innerglow.frag", GL_FRAGMENT_SHADER);
    innerglowShader->link();

    auto lineShader = new Shader();
    lineShader->addSourceAndCompile("./rsc/shader/common/poly2.vert", GL_VERTEX_SHADER);
    lineShader->addSourceAndCompile("./rsc/shader/line.geom", GL_GEOMETRY_SHADER);
    lineShader->addSourceAndCompile("./rsc/shader/fill.frag", GL_FRAGMENT_SHADER);
    lineShader->link();

    auto polylineShader = new Shader();
    polylineShader->addSourceAndCompile("./rsc/shader/polyline.vert", GL_VERTEX_SHADER);
    polylineShader->addSourceAndCompile("./rsc/shader/polyline.geom", GL_GEOMETRY_SHADER);
    polylineShader->addSourceAndCompile("./rsc/shader/fill.frag", GL_FRAGMENT_SHADER);
    polylineShader->link();

    auto bezierShader = new Shader();
    bezierShader->addSourceAndCompile("./rsc/shader/common/poly2.vert", GL_VERTEX_SHADER);
    bezierShader->addSourceAndCompile("./rsc/shader/bezier.geom", GL_GEOMETRY_SHADER);
    bezierShader->addSourceAndCompile("./rsc/shader/fill.frag", GL_FRAGMENT_SHADER);
    bezierShader->link();

    auto circleShader = new Shader();
    circleShader->addSourceAndCompile("./rsc/shader/common/poly2uv.vert", GL_VERTEX_SHADER);
    circleShader->addSourceAndCompile("./rsc/shader/circle.frag", GL_FRAGMENT_SHADER);
    circleShader->link();

    auto computerFont = new FontFamily("./rsc/fonts/CMUNRM.TTF");
    computerFont->addStyle(FontStyle::BOLD, "./rsc/fonts/CMUNBX.TTF");
    computerFont->addStyle(FontStyle::ITALIC, "./rsc/fonts/CMUNTI.TTF");

    auto rodinFont = new FontFamily("./rsc/fonts/Rodin-Regular.ttf");
    rodinFont->addStyle(FontStyle::BOLD, "./rsc/fonts/Rodin-Bold.ttf");
    rodinFont->addStyle(FontStyle::ITALIC, "./rsc/fonts/Rodin-Italic.ttf");
    rodinFont->addStyle(FontStyle::BOLD_ITALIC, "./rsc/fonts/Rodin-BoldItalic.ttf");

    dc = new DC();

    dc->registerShader("texture", textureShader);
    dc->registerShader("text", textureMaskShader);
    dc->registerShader("brush", brushShader);
    dc->registerShader("nebula", nebulaShader);
    dc->registerShader("innerglow", innerglowShader);
    dc->registerShader("line", lineShader);
    dc->registerShader("polyline", polylineShader);
    dc->registerShader("bezier", bezierShader);
    dc->registerShader("circle", circleShader);

    dc->registerFontFamily("computer", computerFont);
    dc->registerFontFamily("rodin", rodinFont);

    dc->setSize((unsigned)w, (unsigned)h);
    dc->init();
}

void Window::render(const FullGraphicData& data) {
    glClearColor(0.94, 0.94, 0.94, 1.f);
    glClear(GL_COLOR_BUFFER_BIT);

    const Color colorPalette[] = {
        Colors::ORANGE,
        Colors::AQUAMARINE,
        Colors::LIGHT_SEA_GREEN,
        Colors::OLIVE,
        Colors::PURPLE,
        Colors::GRAY,
        Colors::YELLOW,
        Colors::RED
    };
    unsigned colorIdx = 0;

    for (const auto& circle : data.circles) {
        dc->drawCircle(circle.position, circle.radius,
            Brush::solid(colorPalette[colorIdx].withAlpha(0.3)));
        colorIdx = (colorIdx + 1) % 8;
    }

    for (const auto& line : data.lines) {
        dc->drawLine(
            line.line.first.toFloat(),
            line.line.second.toFloat(),
            Brush::solid((line.touching) ? Colors::GREEN : Colors::RED),
            1);
    }
}
