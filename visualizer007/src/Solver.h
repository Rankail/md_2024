#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

class Solver {
private:
    std::string filename;
    unsigned edgeInputCount = 0;

    std::vector<std::shared_ptr<PolygonNode>> nodes;
    std::vector<std::shared_ptr<PolygonEdge>> edges;
    std::vector<Polygon> polygons;

    FullGraphicData graphicData;
    Vec2u targetSize;
    int maxScore = 0;

    unsigned totalSteps;

    double zoom = 1.0;
    Vec2d offset = {0.0, 0.0};

public:
    bool init(const std::string& inputFilename);
    void run(unsigned iterations);

    // iterations
    void iteration();

    // update state specific
    void findSmallestNotColliding();
    void bestRotation();
    //void randomizePositions();

    // update stats
    void calculateWorstAndMakeGraphic();

    // output
    void printToFile();

    // get Data
    const std::vector<std::shared_ptr<PolygonNode>>& getNodes() const { return nodes; }
    const FullGraphicData& getGraphicData() const { return graphicData; }

    // camera
    void setTargetSize(Vec2u targetSize);
    void increaseZoom();
    void decreaseZoom();
    void moveOffset(Vec2d offset);
};


#endif //MD_2024_SOLVER_H
