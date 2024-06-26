#ifndef MD_2024_VISUALIZER_H
#define MD_2024_VISUALIZER_H

#include "utils/model.h"
#include "atlas/Window.h"

class Visualizer {
private:
    Window* window;

public:
    Visualizer() = default;
    ~Visualizer();

    bool init(const Vec2u& size = {800, 600});

    void render(const FullGraphicData& data);
};


#endif //MD_2024_VISUALIZER_H
