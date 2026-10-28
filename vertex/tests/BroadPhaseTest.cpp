#include <vertex/BruteForceCollision.h>
#include <vertex/CollisionMethod.h>
#include <vertex/GridCollision.h>
#include <vertex/QuadTreeCollision.h>

#include "Check.h"
#include <cmath>
#include <cstdio>
#include <memory>
#include <random>
#include <vector>

namespace {

using Bodies = std::vector<std::unique_ptr<vertex::BodyPhysics>>;

constexpr float kStepDelta = 0.016f;
constexpr int kSteps = 200;
constexpr float kTwoPi = 6.28318530717958647692f;
constexpr glm::vec2 kBodySize{20.0f, 20.0f};

struct Scenario {
    int bodyCount;
    vertex::Dimensions world;
};

Bodies createBodies(const Scenario& scenario) {
    std::mt19937 rng{1};
    std::uniform_real_distribution<float> randomX{0.0f,
                                                  scenario.world.width - kBodySize.x};
    std::uniform_real_distribution<float> randomY{0.0f,
                                                  scenario.world.height - kBodySize.y};
    std::uniform_real_distribution<float> randomSpeed{40.0f, 160.0f};
    std::uniform_real_distribution<float> randomAngle{0.0f, kTwoPi};

    Bodies bodies;
    for (int i = 0; i < scenario.bodyCount; ++i) {
        auto body = std::make_unique<vertex::BodyPhysics>(
            glm::vec2{randomX(rng), randomY(rng)}, kBodySize);
        const float angle = randomAngle(rng);
        const float speed = randomSpeed(rng);
        body->setVelocity({speed * std::cos(angle), speed * std::sin(angle)});
        bodies.push_back(std::move(body));
    }
    return bodies;
}

void update(Bodies& bodies, const vertex::Dimensions& world) {
    for (auto& body : bodies) {
        body->update(kStepDelta, world);
    }
}

std::vector<std::unique_ptr<vertex::CollisionMethod>> methodsUnderTest() {
    std::vector<std::unique_ptr<vertex::CollisionMethod>> methods;
    methods.push_back(std::make_unique<vertex::BruteForceCollision>());
    methods.push_back(std::make_unique<vertex::GridCollision>());
    methods.push_back(std::make_unique<vertex::QuadTreeCollision>());
    return methods;
}

void checkMatchesBruteForce(vertex::CollisionMethod& method, const Scenario& scenario) {
    Bodies expected = createBodies(scenario);
    Bodies actual = createBodies(scenario);
    vertex::BruteForceCollision brute;
    int knockbacks = 0;

    for (int step = 0; step < kSteps; ++step) {
        brute.resolve(expected);
        method.resolve(actual);
        update(expected, scenario.world);
        update(actual, scenario.world);

        for (std::size_t i = 0; i < expected.size(); ++i) {
            const glm::vec2 want = expected[i]->getPosition();
            const glm::vec2 got = actual[i]->getPosition();
            const bool matches =
                want == got && expected[i]->isKnockedBack() == actual[i]->isKnockedBack();
            if (!matches) {
                std::fprintf(stderr,
                             "%s, %d bodies: body %zu differs at step %d "
                             "(expected %.4f,%.4f got %.4f,%.4f)\n",
                             method.name(), scenario.bodyCount, i, step, want.x, want.y,
                             got.x, got.y);
            }
            CHECK(matches);
            if (expected[i]->isKnockedBack()) {
                ++knockbacks;
            }
        }
    }

    // Without this a scenario too sparse to collide would pass trivially.
    CHECK(knockbacks > 0);
}

void allMethodsMatchBruteForce() {
    const std::vector<Scenario> scenarios = {
        {10, {200, 200}},
        {200, {800, 600}},
        {300, {400, 300}},
        {1000, {2000, 1080}},
    };

    for (const Scenario& scenario : scenarios) {
        for (auto& method : methodsUnderTest()) {
            checkMatchesBruteForce(*method, scenario);
        }
    }
}

}

int main() {
    allMethodsMatchBruteForce();

    std::printf("BroadPhase: all tests passed\n");
    return 0;
}
