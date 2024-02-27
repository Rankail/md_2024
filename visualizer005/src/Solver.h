#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

class Solver {
private:
    std::vector<Node> nodes;
    std::vector<Node> original;
    std::vector<std::vector<bool>> edges;
    unsigned edgeInputCount = 0;
    FullGraphicData graphicData;
    Vec2u targetSize;
    bool doOverlap = true;
    bool doDistance = true;
    bool doAngle = true;

public:
    bool init(const FullInputData* inputData);
    void run(unsigned iterations = 500);

    void iteration();
    void updateWorstDistance();
    void updateWorstOverlap();
    void updateWorstAngle();

    void findSmallestNotColliding();
    void bestRotation();

    void calculateWorstAndMakeGraphic();


    const std::vector<Node>& getNodes() const { return nodes; }
    const FullGraphicData& getGraphicData() const { return graphicData; }

    void setTargetSize(Vec2u targetSize);

    void toggleOverlap();
    void toggleDistance();
    void toggleAngle();

};


#endif //MD_2024_SOLVER_H
