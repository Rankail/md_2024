#include <SDL.h>

#include "Logger.h"
#include "input/InputReader.h"
#include "output/OutputPrinter.h"
#include "atlas/Visualizer.h"

int main(int argc, char** argv) {
    Logger::init();

    auto data = InputReader::readFromFile("input_files/Area_Afro-Eurasia.txt");

    OutputPrinter::printToFile("output_files/Area_Afro-Eurasia.txt", data->nodes);

    auto visualizer = Visualizer();
    visualizer.init();
    visualizer.render(*data);

    return 0;
}
