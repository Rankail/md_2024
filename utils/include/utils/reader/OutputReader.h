#ifndef MD_2024_OUTPUTREADER_H
#define MD_2024_OUTPUTREADER_H

#include "../model.h"

class OutputReader {
public:
    static std::vector<Node>* readFromFile(const std::string& path);

private:
    static Node readNodeLine(const std::string& line, unsigned lineNum);
};


#endif //MD_2024_OUTPUTREADER_H
