#ifndef MD_2024_INPUTREADER_H
#define MD_2024_INPUTREADER_H

#include <unordered_map>

#include "model.h"

class InputReader {
public:
    static FullInputData* readFromFile(const std::string& path);

private:
    static Node* readNodeLine(const std::string& line, unsigned lineNum);
    static uPair readEdgeLine(const std::string& line, const std::unordered_map<std::string, unsigned>& nameIdxMapping);
};


#endif //MD_2024_INPUTREADER_H
