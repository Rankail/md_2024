#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

#include "model2.h"

class Solver {
private:
    std::vector<GraphNode*> nodes;

public:
    bool init(const FullInputData* inputData);
    void run();

    void place(GraphNode* node);

    std::vector<Node> getNodes(const std::vector<Node>& originalNodes);

};


#endif //MD_2024_SOLVER_H
