#ifndef MD_2024_OUTPUTPRINTER_H
#define MD_2024_OUTPUTPRINTER_H

#include <utils/model.h>

class OutputPrinter {
public:
    static void printToFile(const std::string& path, std::vector<Node> nodes);

};


#endif //MD_2024_OUTPUTPRINTER_H
