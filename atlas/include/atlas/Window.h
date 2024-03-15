#ifndef MD_2024_WINDOW_H
#define MD_2024_WINDOW_H

#include <SDL.h>
#include <string>

#include "WindowFlag.h"
#include "utils/math/vec.h"
#include "utils/model.h"
#include "atlas/render/DC.h"

class Window {
protected:
    std::string caption;

    SDL_Window* handle;
    SDL_GLContext context;

    DC* dc = nullptr;

    Vec2u size;

    uint32_t prevTime;

public:
    explicit Window(const std::string& title, const Vec2u& size = {800, 600});
    explicit Window(const std::string& title, Vec2i position, Vec2u size, WindowFlag flags);
    virtual ~Window();

    virtual void init();
    void render(const FullGraphicData& data);
    void render(const FullGraphicDataWorstEdges& data);
    void render(const FullGraphicData2& data);
    void renderBasicGraphicData(const FullGraphicData& data, bool drawLines = true);
    void renderStats(const FullGraphicData& data);

    void close();

    void setCaption(const std::string &capt);
    std::string getCaption() const { return caption; }

    SDL_Window*   getHandle()  const { return handle; }
    SDL_GLContext getContext() const { return context; }

    void handleEvent(const SDL_Event& event);

    Vec2u getSize() const { return size; }
};


#endif //MD_2024_WINDOW_H
