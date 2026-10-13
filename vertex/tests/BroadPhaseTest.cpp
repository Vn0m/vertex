#include <cstdio>
#include <memory>
#include <vector>

#include "Check.h"
#include "physics/BruteForceCollision.h"
#include "physics/CollisionMethod.h"
#include "physics/GridCollision.h"
#include "physics/PhysicsStressTest.h"
#include "physics/QuadTreeCollision.h"

// Verifies every collision method produces exactly the same simulation as brute force.

namespace {

using vertex::BodyPhysics;
using vertex::tests::BruteForceCollision;
using vertex::tests::CollisionMethod;
using vertex::tests::GridCollision;
using vertex::tests::QuadTreeCollision;
using vertex::tests::StressTestConfig;
using Bodies = std::vector<std::unique_ptr<BodyPhysics>>;

constexpr float kStepDelta = 0.016f;
constexpr int kSteps = 200;

// Fresh instances of every method under test
std::vector<std::unique_ptr<CollisionMethod>> methodsUnderTest() {
    std::vector<std::unique_ptr<CollisionMethod>> methods;
    methods.push_back(std::make_unique<BruteForceCollision>());
    methods.push_back(std::make_unique<GridCollision>());
    methods.push_back(std::make_unique<QuadTreeCollision>());
    return methods;
}

StressTestConfig scenario(int entityCount, vertex::Dimensions windowSize) {
    StressTestConfig config;
    config.entityCount = entityCount;
    config.windowSize = windowSize;
    return config;
}

// Runs brute force and `method` side by side
void checkMatchesBruteForce(CollisionMethod& method, const StressTestConfig& config) {
    Bodies expected = vertex::tests::createStressTestBodies(config);
    Bodies actual = vertex::tests::createStressTestBodies(config);
    BruteForceCollision brute;
    int knockbacks = 0;

    for (int step = 0; step < kSteps; ++step) {
        brute.resolve(expected);
        method.resolve(actual);
        vertex::tests::updateStressTestBodies(expected, kStepDelta, config.windowSize);
        vertex::tests::updateStressTestBodies(actual, kStepDelta, config.windowSize);

        for (std::size_t i = 0; i < expected.size(); ++i) {
            const glm::vec2 want = expected[i]->getPosition();
            const glm::vec2 got = actual[i]->getPosition();
            const bool matches =
                want == got && expected[i]->isKnockedBack() == actual[i]->isKnockedBack();
            if (!matches) {
                std::fprintf(stderr,
                             "%s, %d bodies: body %zu differs at step %d "
                             "(expected %.4f,%.4f got %.4f,%.4f)\n",
                             method.name(), config.entityCount, i, step, want.x, want.y,
                             got.x, got.y);
            }
            CHECK(matches);
            if (expected[i]->isKnockedBack()) {
                ++knockbacks;
            }
        }
    }

    // Guards against a scenario too sparse to exercise collisions
    CHECK(knockbacks > 0);
}

void allMethodsMatchBruteForce() {
    const std::vector<StressTestConfig> scenarios = {
        scenario(10, {200, 200}),
        scenario(200, {800, 600}),
        scenario(300, {400, 300}),
        scenario(1000, {2000, 1080}),
    };

    for (const StressTestConfig& config : scenarios) {
        for (auto& method : methodsUnderTest()) {
            checkMatchesBruteForce(*method, config);
        }
    }
}

}

int main() {
    allMethodsMatchBruteForce();

    std::printf("BroadPhase: all tests passed\n");
    return 0;
}
