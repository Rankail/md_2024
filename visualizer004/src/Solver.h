#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>
#include <box2d/box2d.h>

class Solver {
private:
    std::vector<Node> original;
    std::vector<Node> nodes;
    std::vector<std::vector<bool>> edges;

    std::vector<b2Body*> bodies;
    std::vector<b2DistanceJoint*> joints;

    b2World* world;

public:
    bool init(const FullInputData* inputData);
    void run(unsigned iterations = 500);

    void iteration();

    const std::vector<Node>& getNodes() const { return nodes; }

    void printScore();

    void findSmallestNotColliding();

    void updateNodesWithSimulation();

};


#endif //MD_2024_SOLVER_H
