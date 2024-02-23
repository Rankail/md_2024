#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

class Solver {
private:
    std::vector<Node> nodes;
    std::vector<Node> original;
    std::vector<std::vector<bool>> edges;

public:
    bool init(const FullInputData* inputData);
    void run(unsigned iterations = 500);
    void printResult();

    void iteration();

    const std::vector<Node>& getNodes() const { return nodes; }

};


#endif //MD_2024_SOLVER_H
