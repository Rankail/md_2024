#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

class Solver {
private:
    std::string filename;
    unsigned edgeInputCount = 0;

    std::vector<Node> nodes;
    std::vector<Node> original;
    std::vector<std::vector<bool>> edges;

    FullGraphicData2 graphicData;
    Vec2u targetSize;
    int maxScore = 0;

    unsigned totalSteps;

    double overlapFactor = 0.1;
    double distanceFactor = 0.1;
    double angleFactor = 0.1;
    double zoom = 1.0;
    Vec2d offset = {0.0, 0.0};

public:
    bool init(const std::string& inputFilename);
    void run(unsigned iterations);

    // iterations
    void iteration();
    void updateDistances();
    void updateWorstOverlap();
    void updateAngles();

    // update state specific
    void findSmallestNotColliding();
    void bestRotation();
    void randomizePositions();

    // update stats
    void calculateWorstAndMakeGraphic();

    // output
    void printToFile();

    // get Data
    const std::vector<Node>& getNodes() const { return nodes; }
    const FullGraphicData2& getGraphicData() const { return graphicData; }

    // camera
    void setTargetSize(Vec2u targetSize);
    void increaseZoom();
    void decreaseZoom();
    void moveOffset(Vec2d offset);

    // change Factors
    void strengthenDistance();
    void weakenDistance();
    void strengthenOverlap();
    void weakenOverlap();
    void strengthenAngle();
    void weakenAngle();
};


#endif //MD_2024_SOLVER_H
