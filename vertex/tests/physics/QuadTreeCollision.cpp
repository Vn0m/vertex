#include "QuadTreeCollision.h"

#include <algorithm>

#include <glm/common.hpp>

namespace vertex::tests {

void QuadTreeCollision::resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) {
    if (bodies.size() < 2) {
        return;
    }


    glm::vec2 minCorner = bodies[0]->getPosition();
    glm::vec2 maxCorner = minCorner;
    float largestSide = 0.0f;
    for (const auto& body : bodies) {
        const glm::vec2 position = body->getPosition();
        const glm::vec2 size = body->getSize();
        minCorner = glm::min(minCorner, position);
        maxCorner = glm::max(maxCorner, position + size);
        largestSide = std::max({largestSide, size.x, size.y});
    }
    // Leaves stop splitting near the body size,
    mTree.reset(minCorner, maxCorner - minCorner,
                std::max(largestSide * kMinNodeSizeFactor, 1.0f));

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        mTree.insert(static_cast<std::uint32_t>(i), bodies[i]->getPosition(),
                     bodies[i]->getSize());
    }

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        mTree.query(bodies[i]->getPosition(), bodies[i]->getSize(), mNeighbors);
        for (const std::uint32_t j : mNeighbors) {
            if (j > i) {
                bodies[i]->resolveCollision(*bodies[j]);
            }
        }
    }
}

}
