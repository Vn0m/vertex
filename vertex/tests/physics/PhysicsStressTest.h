#pragma once

#include <memory>
#include <vector>

#include "vertex/Physics.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"
#include "vertex/types.h"

namespace vertex::tests {

// Instructions for running this test from test/physics
//  modify entity count
struct StressTestConfig {
    int entityCount{100};
    Dimensions windowSize{1280, 960};
    glm::vec2 bodySize{20.0f, 20.0f};
    float minSpeed{40.0f};
    float maxSpeed{160.0f};
    // Fixed seed
    unsigned int seed{1};
};

// generate position and movement values
std::vector<std::unique_ptr<BodyPhysics>> createStressTestBodies(
    const StressTestConfig& config);

// Collisions are resolved by a CollisionMethod (see CollisionMethod.h), chosen on the
// command line, so each broad phase can be timed and verified separately.

// Advances each body by deltaTime, bouncing off the window edges.
void updateStressTestBodies(std::vector<std::unique_ptr<BodyPhysics>>& bodies,
                            float deltaTime, const Dimensions& windowSize);

// Draws every body as a tinted sprite (red while knocked back, white otherwise).
void drawStressTest(Renderer& renderer, Texture& tex,
                    const std::vector<std::unique_ptr<BodyPhysics>>& bodies);

}
