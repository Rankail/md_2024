#include "Visualizer.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <utils/log/Logger.h>

Visualizer::~Visualizer() {
    delete window;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();
}

bool Visualizer::init(const Vec2u& size) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        TET_ERROR("Failed to initialize SDL! {}", SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
    SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 4);

    int imgFlags = IMG_INIT_PNG;
    if (IMG_Init(imgFlags) != imgFlags) {
        TET_ERROR("Failed to initialize SDL_image. {}", IMG_GetError());
        return false;
    }

    if (TTF_Init() < 0) {
        TET_ERROR("Failed to initialized SDL_ttf. {}", TTF_GetError());
        return false;
    }

    window = new Window("MD 2024", {-1, -1}, size, WindowFlag::SHOWN);

    gladLoadGLLoader(SDL_GL_GetProcAddress);

    window->init();

    return true;
}

void Visualizer::render(const FullGraphicData& data) {
    bool quit = false;
    while (!quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch(event.type) {
                case SDL_QUIT:
                    quit = true;
                    break;
            }
        }
        SDL_GL_MakeCurrent(window->getHandle(), window->getContext());
        window->render(data);
        SDL_GL_SwapWindow(window->getHandle());
    }
}
