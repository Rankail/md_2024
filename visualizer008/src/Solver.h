#ifndef MD_2024_SOLVER_H
#define MD_2024_SOLVER_H

#include <utils/model.h>

class Solver {
private:
    std::string filename;
    unsigned edgeInputCount = 0;

    std::vector<Node> nodes;
    std::vector<Vec2d> angleGradients;
    std::vector<Vec2d> distGradients;
    std::vector<Vec2d> overlapGradients;
    std::vector<Node> original;
    std::vector<std::vector<bool>> edges;

    FullGraphicData graphicData;
    Vec2u targetSize;
    int maxScore = 0;

    unsigned totalSteps;

    double stepSize = 1.0;

    double zoom = 1.0;
    Vec2d offset = {0.0, 0.0};

public:
    bool init(const std::string& inputFilename);
    void run(unsigned iterations);

    // iterations
    void iteration();

    void calculateOverlapDerivatives();
    void calculateDistDerivatives();
    void calculateAngleDerivatives();
    Vec2d calculateAngleDerivative(Vec2d nDiff, Vec2d oDiff);
    Vec2d calculateAngleDerivative2(Vec2d nDiff, Vec2d oDiff);

    void applyGradients(const std::vector<Vec2d>& gradients, double stepSize = 1.0);

    // update state specific
    void findSmallestNotColliding();
    void bestRotation();
    void randomizePositions();

    // update stats
    void makeGraphic(bool updateScore = true);
    void calculateScore();

    // output
    void printToFile();

    // get Data
    const std::vector<Node>& getNodes() const { return nodes; }
    const FullGraphicData& getGraphicData() const { return graphicData; }

    // camera
    void setTargetSize(Vec2u targetSize);
    void increaseZoom();
    void decreaseZoom();
    void moveOffset(Vec2d offset);

    void increaseStepSize();
    void decreaseStepSize();
};


#endif //MD_2024_SOLVER_H
