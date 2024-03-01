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
    inputFilename = inputFilePath.substr(lastSlash+1);

    auto inputData = InputReader::readFromFile(inputFilePath);

    edges = inputData->edges;

    solver.init(inputData);

    graphicData = getGraphicDataFromData(solver.getNodes(), edges);

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

FullGraphicData Visualizer::getGraphicDataFromData(const std::vector<Node> &inputNodes, const std::vector<uPair> &edges) {
    auto minX = std::numeric_limits<double>::max();
    auto minY = std::numeric_limits<double>::max();
    auto maxX = std::numeric_limits<double>::min();
    auto maxY = std::numeric_limits<double>::min();

    for (const auto& circle : inputNodes) {
        minX = std::min(minX, circle.position.x() - circle.radius);
        minY = std::min(minY, circle.position.y() - circle.radius);
        maxX = std::max(maxX, circle.position.x() + circle.radius);
        maxY = std::max(maxY, circle.position.y() + circle.radius);
    }
    Vec2d positionOffset = Vec2d(minX, maxY);
    Vec2u targetSize = window->getSize();
    double scaleFactor = std::min(1 / (maxX - minX) * targetSize.x(), 1 / (maxY - minY) * targetSize.y());

    auto graphicData = FullGraphicData();
    for (const auto& circle : inputNodes) {
        auto newCircle = circle;
        newCircle.position -= positionOffset;
        newCircle.position.x() *= scaleFactor;
        newCircle.position.y() *= -scaleFactor;
        newCircle.radius *= scaleFactor;
        graphicData.circles.emplace_back(newCircle);
    }

    for (const auto& [idx1, idx2] : edges) {
        if (idx2 < idx1) continue;
        bool touching = false;
        const auto& node1 = graphicData.circles[idx1];
        const auto& node2 = graphicData.circles[idx2];
        const auto diff = (node1.position - node2.position).abs();
        const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
        const auto c2 = std::pow(node1.radius + node2.radius, 2.);
        if (a2b2 <= c2) {
            touching = true;
        }
        auto edge = GraphicEdge();
        edge.touching = touching;
        edge.idxs = {idx1, idx2};
        graphicData.edges.emplace_back(edge);
    }
    return graphicData;
}

void Visualizer::onKeyDown(const SDL_Event &event) {
    switch(event.key.keysym.scancode) {
        case SDL_SCANCODE_A: {
            solver.iteration();
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_S: {
            solver.run(10);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_D: {
            solver.run(100);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_F: {
            solver.run(1000);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_G: {
            solver.run(10000);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        default:
            break;
    }
}

void Visualizer::onKeyUp(const SDL_Event &event) {
    TET_INFO("Physical {} key acting as {} key",
        SDL_GetScancodeName(event.key.keysym.scancode),
        SDL_GetKeyName(event.key.keysym.sym));
    switch (event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            quit = true;
            break;

        case SDL_SCANCODE_Q: {
            solver.run(1);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_W: {
            solver.run(10);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_E: {
            solver.run(100);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_R: {
            solver.run(1000);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_T: {
            solver.run(10000);
            graphicData = getGraphicDataFromData(solver.getNodes(), edges);
            break;
        }
        case SDL_SCANCODE_P: {
            OutputPrinter::printToFile(std::string(MD_OUTPUT_DIR) + "/" + inputFilename + ".out", solver.getNodes());
            TET_INFO("Printed");
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
        case SDL_SCANCODE_M: solver.printScore(); break;
        default:
            break;
    }
}

void Visualizer::switchTo(char c) {
    auto inputFilePath = selectFileFromDir(MD_INPUT_DIR, c);

    const auto lastSlash = inputFilePath.find_last_of("/\\");
    inputFilename = inputFilePath.substr(lastSlash+1);

    auto inputData = InputReader::readFromFile(inputFilePath);

    edges = inputData->edges;

    solver.init(inputData);

    graphicData = getGraphicDataFromData(solver.getNodes(), edges);
}
