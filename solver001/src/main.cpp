#include <iostream>
#include <utils/utils.h>
#include <utils/log/Logger.h>
#include <utils/reader/InputReader.h>
#include <utils/printer/OutputPrinter.h>
#include <utils/Arguments.h>
#include "Solver.h"

int main(int argc, char** argv) {
    Logger::init();

    Arguments args{argc, argv};
    char filePreselection = '\0';
    if (args.getArgCount() == 1) {
        args >> filePreselection;
    }

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR, filePreselection);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& filename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;

    auto inputData = InputReader::readFromFile(inputFilePath);

    TET_TRACE("Parsed input in {}s", sw);
    sw.reset();

    Solver solver{};
    solver.init(inputData);

    unsigned iterations;
    auto itStr = args.getNamed("i");
    if (itStr.has_value()) {
        iterations = std::stoul(itStr.value());
    } else {
        std::cout << "Iterations: ";
        std::cin >> iterations;
    }
    
    sw.reset();
    solver.run(iterations);

    TET_INFO("{} iterations in {}s", iterations, sw);
    sw.reset();

    auto nodes = solver.getNodes();

    OutputPrinter::printToFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out", nodes);

    return 0;
}