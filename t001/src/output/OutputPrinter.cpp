#include "OutputPrinter.h"

#include <fstream>
#include <filesystem>
#include <ranges>

#include "../Logger.h"

void OutputPrinter::printToFile(const std::string& path, std::vector<Node*> nodes) {
    std::ofstream file(path, std::ios::out | std::ios::trunc);

    if (!file.is_open()) {
        TET_ERROR("Unable to open file '{}' executed from '{}'", path, std::filesystem::current_path().generic_string());
        file.close();
        return;
    }

    std::sort(nodes.begin(), nodes.end(), [](const Node *a, const Node* b) { return a->index < b->index; });

    for (const auto node : nodes) {
        file << node->originalPosition.x() << " " << node->originalPosition.y() << " " << node->weight << " " << node->name << " " << node->index << std::endl;
    }

    file.close();
}
