#include "physics/BruteForceCollision.h"

namespace vertex::tests {

// @brief resolve the collision by checking all pairs for all objects
void BruteForceCollision::resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        for (std::size_t j = i + 1; j < bodies.size(); ++j) {
            bodies[i]->resolveCollision(*bodies[j]);
        }
    }
}
}
