#include "Solver.h"

#include <algorithm>

#include <box2d/box2d.h>

bool Solver::init(const FullInputData* inputData) {

    if (world != nullptr) {
        for (auto& joint : joints) {
            world->DestroyJoint(joint);
        }
        for (auto& body : bodies) {
            world->DestroyBody(body);
        }
        delete world;
    }

    original.clear();
    nodes.clear();
    bodies.clear();
    edges.clear();

    b2Vec2 gravity(0.0f, 0.0f);
    world = new b2World(gravity);

    for (const Node* node : inputData->nodes) {
        original.emplace_back(*node);
        nodes.emplace_back(*node);

        b2BodyDef bodyDef;
        bodyDef.type = b2_dynamicBody;
        bodyDef.position.Set(node->position.x(), node->position.y());
        b2Body* body = world->CreateBody(&bodyDef);

        b2CircleShape shape;
        shape.m_p.Set(0.0f, 0.f);
        shape.m_radius = node->radius;

        b2FixtureDef fixtureDef;
        fixtureDef.shape = &shape;
        fixtureDef.density = 1.f;
        fixtureDef.friction = 0.f;
        body->CreateFixture(&fixtureDef);

        bodies.emplace_back(body);
    }

    edges = std::vector<std::vector<bool>>(original.size(), std::vector<bool>(original.size(), false));

    for (const auto [idx1, idx2] : inputData->edges) {
        if (idx1 >= idx2) continue;

        edges[idx1][idx2] = true;
        edges[idx2][idx1] = true;

        b2DistanceJointDef jointDef;
        jointDef.Initialize(bodies[idx1], bodies[idx2], bodies[idx1]->GetLocalCenter(), bodies[idx2]->GetLocalCenter());
        jointDef.collideConnected = true;
        jointDef.length = original[idx1].radius + original[idx2].radius;
        b2LinearStiffness(jointDef.stiffness, jointDef.damping, 1.f, 0.99f, bodies[idx1], bodies[idx2]);

        b2DistanceJoint* joint = (b2DistanceJoint*)world->CreateJoint(&jointDef);
        joints.emplace_back(joint);
    }

    return true;
}

void Solver::run(unsigned iterations) {
    for (int i = 0; i < iterations; i++) {
        iteration();
    }
}

void Solver::findSmallestNotColliding() {
    double maxOverlap = 0.0;

    for (int i = 0; i < nodes.size()-1; i++) {
        const auto& n1 = nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n2 = nodes[j];
            const auto diff = n1.position - n2.position;
            const auto dist = std::hypot(diff.x(), diff.y());
            const auto r12 = n1.radius + n2.radius;
            const auto overlap = (r12 - dist) / dist;
            maxOverlap = std::max(maxOverlap, overlap);
        }
    }


    //const auto avgOverlap = sumOverlap / (nodes.size() * (nodes.size() - 1) / 2.);

    const auto factor = 1 + maxOverlap;
    for (int i = 0; i < nodes.size(); i++) {
        nodes[i].position *= factor;
    }
}

void Solver::iteration() {
    world->Step(1./60., 6, 2);
    updateNodesWithSimulation();
}

void Solver::printScore() {
    double maxDistance = 0.0;
    double maxOverlap = 0.0;
    double maxAngle = 0.0;

    for (int i = 0; i < nodes.size()-1; i++) {
        const auto& n1 = nodes[i];
        for (int j = i + 1; j < nodes.size(); j++) {
            const auto& n2 = nodes[j];
            const auto diff = n1.position - n2.position;
            const auto a2b2 = diff.x() * diff.x() + diff.y() * diff.y();
            const auto r12 = n1.radius + n2.radius;
            const auto c2 = r12 * r12;
            if (a2b2 < c2) {
                const auto dist = std::sqrt(a2b2);
                const auto overlap = (r12 - dist) / r12;
                maxOverlap = std::max(maxOverlap, overlap);
            }
            if (edges[i][j]) {
                if (a2b2 > c2) {
                    const auto dist = std::sqrt(a2b2);
                    const auto unwantedDist = (dist - r12) / r12;
                    maxDistance = std::max(maxDistance, unwantedDist);
                }

                const auto in1 = original[i];
                const auto in2 = original[j];
                const auto inDiff = in1.position - in2.position;
                const auto angle = radToDeg(inDiff.smallestAngleTo(diff));
                maxAngle = std::max(maxAngle, (double)angle);
            }
        }
    }

    const auto overlapFactor = maxOverlap * 200;
    const auto distanceFactor = maxDistance * 100;
    const auto angleFactor = maxAngle / 18.0;

    const auto score = 1000.0 * (nodes.size() + edges.size()) / (1 + overlapFactor + distanceFactor + angleFactor);
    const auto scoreRounded = (int)(score + 0.5);

    std::cout   << "Max overlap: " << maxOverlap << std::endl
                << "Max distance: " << maxDistance << std::endl
                << "Max angle: " << maxAngle << std::endl
                << "Score: " << scoreRounded << std::endl;
}

void Solver::updateNodesWithSimulation() {
    for (int i = 0; i < bodies.size(); i++) {
        const auto& body = bodies[i];
        const auto pos = body->GetPosition();
        nodes[i].position = {pos.x, pos.y};
    }
}
