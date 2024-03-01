#include "Visualizer.h"

#include <SDL.h>
#include <SDL_ttf.h>
#include <SDL_image.h>

#include <utils/log/Logger.h>
#include <utils/reader/InputReader.h>
#include <utils/printer/OutputPrinter.h>
#include <ranges>

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

    quit = false;

    return true;
}

void Visualizer::run(const std::string& inputFilePath) {
    const auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto filename = inputFilePath.substr(lastSlash+1);

    solver.setTargetSize(window->getSize());

    solver.init(filename);

    graphicData = solver.getGraphicData();

    while (!quit) {
        handleEvents();

        SDL_GL_MakeCurrent(window->getHandle(), window->getContext());
        window->render(graphicData);
        SDL_GL_SwapWindow(window->getHandle());
    }
}

void Visualizer::handleEvents() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                quit = true;
                break;
            case SDL_WINDOWEVENT:
                window->handleEvent(event);
                if (event.window.type == SDL_WINDOWEVENT_RESIZED) {
                    solver.setTargetSize({event.window.data1, event.window.data2});
                }
                break;
            case SDL_KEYDOWN:
                onKeyDown(event);
                break;
            case SDL_KEYUP:
                onKeyUp(event);
                break;
        }
    }
}

void Visualizer::onKeyDown(const SDL_Event &event) {
//    TET_INFO("Physical {} key acting as {} key",
//        SDL_GetScancodeName(event.key.keysym.scancode),
//        SDL_GetKeyName(event.key.keysym.sym));
    switch(event.key.keysym.scancode) {
        case SDL_SCANCODE_A: {
            solver.run(1);
            break;
        }
        case SDL_SCANCODE_S: {
            solver.run(10);
            break;
        }
        case SDL_SCANCODE_D: {
            solver.run(100);
            break;
        }
        case SDL_SCANCODE_F: {
            solver.run(1000);
            break;
        }
        case SDL_SCANCODE_G: {
            solver.run(10000);
            break;
        }
        case SDL_SCANCODE_UP: {
            solver.moveOffset({0., -10.});
            break;
        }
        case SDL_SCANCODE_DOWN: {
            solver.moveOffset({0., 10.});
            break;
        }
        case SDL_SCANCODE_LEFT: {
            solver.moveOffset({-10., 0.});
            break;
        }
        case SDL_SCANCODE_RIGHT: {
            solver.moveOffset({10., 0.});
            break;
        }
        case SDL_SCANCODE_RIGHTBRACKET: {
            solver.decreaseZoom();
            break;
        }
        case SDL_SCANCODE_BACKSLASH: {
            solver.increaseZoom();
            break;
        }
        default:
            return;
    }
    graphicData = solver.getGraphicData();
}

void Visualizer::onKeyUp(const SDL_Event &event) {
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            quit = true;
            break;

        case SDL_SCANCODE_Q: {
            solver.run(1);
            break;
        }
        case SDL_SCANCODE_W: {
            solver.run(10);
            break;
        }
        case SDL_SCANCODE_E: {
            solver.run(100);
            break;
        }
        case SDL_SCANCODE_R: {
            solver.run(1000);
            break;
        }
        case SDL_SCANCODE_T: {
            solver.run(10000);
            break;
        }
        case SDL_SCANCODE_P: {
            // solver.printToFile();
            TET_WARN("P(rinting) manually is deprecated");
            break;
        }
        case SDL_SCANCODE_1: switchTo('0'); break;
        case SDL_SCANCODE_2: switchTo('1'); break;
        case SDL_SCANCODE_3: switchTo('2'); break;
        case SDL_SCANCODE_4: switchTo('3'); break;
        case SDL_SCANCODE_5: switchTo('4'); break;
        case SDL_SCANCODE_6: switchTo('5'); break;
        case SDL_SCANCODE_7: switchTo('6'); break;
        case SDL_SCANCODE_8: switchTo('7'); break;
        case SDL_SCANCODE_9: switchTo('8'); break;
        case SDL_SCANCODE_0: switchTo('9'); break;
        case SDL_SCANCODE_MINUS: switchTo('q'); break;
        case SDL_SCANCODE_EQUALS: switchTo('w'); break;
        case SDL_SCANCODE_C: {
            solver.findSmallestNotColliding();
            break;
        }
        case SDL_SCANCODE_U: solver.strengthenDistance(); break;
        case SDL_SCANCODE_I: solver.strengthenOverlap(); break;
        case SDL_SCANCODE_O: solver.strengthenAngle(); break;
        case SDL_SCANCODE_J: solver.weakenDistance(); break;
        case SDL_SCANCODE_K: solver.weakenOverlap(); break;
        case SDL_SCANCODE_L: solver.weakenAngle(); break;
        case SDL_SCANCODE_X: {
            solver.bestRotation();
            break;
        }
        default:
            return;
    }
    graphicData = solver.getGraphicData();
}

void Visualizer::switchTo(char c) {
    auto inputFilePath = selectFileFromDir(MD_INPUT_DIR, c);

    const auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto filename = inputFilePath.substr(lastSlash+1);
    const auto firstPoint = filename.find_first_of('.');
    const auto inputFileWithoutExt = filename.substr(0, firstPoint);

    window->setCaption(inputFileWithoutExt);

    solver.init(filename);
}
