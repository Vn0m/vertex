#include "physics/GridCollision.h"

#include <algorithm>

#include <glm/common.hpp>

namespace vertex::tests {

// @brief collision resolution method for Spatial Grid
// @return none
void GridCollision::resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) {
    // no pairs early return
    if (bodies.size() < 2) {
        return;
    }

    // Bounda pass  begin
    glm::vec2 minCorner = bodies[0]->getPosition();
    glm::vec2 maxCorner = minCorner;
    float largestSide = 0.0f;

    // for each body in the vector build an AABB that encloses the object.
    for (const auto& body : bodies) {
        const glm::vec2 position = body->getPosition();
        const glm::vec2 size = body->getSize();
        minCorner = glm::min(minCorner, position);
        maxCorner = glm::max(maxCorner, position + size);

        // record largest side.
        largestSide = std::max({largestSide, size.x, size.y});
    }
    // Bound pass end

    // rebuild the grid with the dimensinos of the object. kCellSize scale factor
    // is 2* the largest side of the object
    mGrid.reset(minCorner, maxCorner - minCorner,
                std::max(largestSide * kCellSizeFactor, 1.0f));

    // each body is then inserted into the grid, index based.
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        mGrid.insert(static_cast<std::uint32_t>(i), bodies[i]->getPosition(),
                     bodies[i]->getSize());
    }

    // each object querries for a list of nearby objects.
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        mGrid.query(bodies[i]->getPosition(), bodies[i]->getSize(), mNeighbors);

        // based on ID, the objects resolve collision with each
        // nearobject
        for (const std::uint32_t j : mNeighbors) {
            if (j > i) {
                // use of basic collision resolution
                bodies[i]->resolveCollision(*bodies[j]);
            }
        }
    }
}

}
