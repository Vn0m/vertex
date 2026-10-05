#pragma once

#include <memory>
#include <vector>

#include "vertex/Physics.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"
#include "vertex/types.h"

namespace vertex::tests {

// Instructions for running this test from glade/src/main.cpp:
//  modify entity count
struct StressTestConfig {
    int entityCount{100};
    Dimensions windowSize{1280, 960};
    glm::vec2 bodySize{20.0f, 20.0f};
    float minSpeed{40.0f};
    float maxSpeed{160.0f};
    // Fixed seed so a run can be reproduced; pass a different value for a
    // fresh random layout each time.
    unsigned int seed{1};
};

// generate position and movement values 
std::vector<std::unique_ptr<BodyPhysics>> createStressTestBodies(const StressTestConfig& config);

// Resolves collisions for every pair of bodies (O(n^2), fine up to a few
// thousand bodies) and then advances each body by deltaTime.
void stepStressTest(std::vector<std::unique_ptr<BodyPhysics>>& bodies, float deltaTime,
                    const Dimensions& windowSize);

// Draws every body as a tinted sprite (red while knocked back, white otherwise).
void drawStressTest(Renderer& renderer, Texture& tex,
                    const std::vector<std::unique_ptr<BodyPhysics>>& bodies);

}
