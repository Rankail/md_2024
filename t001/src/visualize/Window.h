#ifndef MD_2024_WINDOW_H
#define MD_2024_WINDOW_H

#include <SDL.h>
#include <string>

#include "WindowFlag.h"
#include "vec.h"
#include "visualize/render/DC.h"

class Window {
protected:
    std::string caption;

    SDL_Window* handle;
    SDL_GLContext context;

    DC* dc = nullptr;

public:
    explicit Window(const std::string& title, Vec2u size = {800, 600});
    explicit Window(const std::string& title, Vec2u position, Vec2u size, WindowFlag flags);
    virtual ~Window();

    virtual void init();
    virtual void render();

    void close();

    void setCaption(const std::string &capt);
    std::string getCaption() const { return caption; }

    SDL_Window*   getHandle()  const { return handle; }
    SDL_GLContext getContext() const { return context; }
};


#endif //MD_2024_WINDOW_H
