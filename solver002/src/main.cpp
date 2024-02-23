#include <iostream>
#include <utils/utils.h>
#include <utils/log/Logger.h>
#include <utils/reader/InputReader.h>
#include "Solver.h"
#include "output/OutputPrinter.h"

int main(int argc, char** argv) {
    Logger::init();

    const std::string inputFilePath = selectFileFromDir(MD_INPUT_DIR);
    std::cout << inputFilePath << std::endl;

    auto lastSlash = inputFilePath.find_last_of("/\\");
    const auto& filename = inputFilePath.substr(lastSlash+1);

    spdlog::stopwatch sw;

    auto inputData = InputReader::readFromFile(inputFilePath);

    TET_TRACE("Parsed input in {}s", sw);
    sw.reset();

    Solver solver{};
    solver.init(inputData);
    solver.run(100000);

    TET_INFO("100k iterations in {}s", sw);
    sw.reset();

    auto nodes = solver.getNodes();

    OutputPrinter::printToFile(std::string(MD_OUTPUT_DIR) + "/" + filename + ".out", nodes);

    return 0;
}