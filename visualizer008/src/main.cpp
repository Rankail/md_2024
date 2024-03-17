#include <iostream>
#include <utils/utils.h>
#include <utils/log/Logger.h>
#include "Visualizer.h"
#include "utils/Arguments.h"

int main(int argc, char** argv) {
    Logger::init();

    Arguments args{argc, argv};
    char filePreselection = '\0';
    if (args.getArgCount() == 1) {
        args >> filePreselection;
    }

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR, filePreselection);
    std::cout << inputFilePath << std::endl;

    Visualizer visualizer{};
    visualizer.init({1600, 900});
    visualizer.run(inputFilePath);

    return 0;
}