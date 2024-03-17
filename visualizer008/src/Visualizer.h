#ifndef MD_2024_VISUALIZER_H
#define MD_2024_VISUALIZER_H

#include "utils/model.h"
#include "atlas/Window.h"

#include "Solver.h"

class Visualizer {
private:
    Window* window;
    bool quit = false;
    Solver solver;
    FullGraphicData graphicData;

public:
    Visualizer() = default;
    ~Visualizer();

    bool init(const Vec2u& size = {800, 600});

    void run(const std::string& inputFilePath);

    void handleEvents();

    void onKeyDown(const SDL_Event& event);
    void onKeyUp(const SDL_Event& event);

    void switchTo(char c);
};


#endif //MD_2024_VISUALIZER_H
