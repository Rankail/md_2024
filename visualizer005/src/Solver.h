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

    FullGraphicData graphicData;
    Vec2u targetSize;
    int maxScore = 0;

    double overlapFactor = 0.1;
    double distanceFactor = 0.1;
    double angleFactor = 0.1;
    double zoom = 1.0;
    Vec2d offset = {0.0, 0.0};


public:
    bool init(const std::string& inputFilename);
    void run(unsigned iterations);

    void iteration();
    void updateWorstDistance();
    void updateWorstOverlap();
    void updateWorstAngle();

    void findSmallestNotColliding();
    void bestRotation();

    void calculateWorstAndMakeGraphic();

    void printToFile();


    const std::vector<Node>& getNodes() const { return nodes; }
    const FullGraphicData& getGraphicData() const { return graphicData; }

    void setTargetSize(Vec2u targetSize);
    void increaseZoom();
    void decreaseZoom();
    void moveOffset(Vec2d offset);

    void strengthenDistance();
    void weakenDistance();
    void strengthenOverlap();
    void weakenOverlap();
    void strengthenAngle();
    void weakenAngle();
};


#endif //MD_2024_SOLVER_H
