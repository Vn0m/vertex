#include "PhysicsStressTest.h"

#include <cmath>
#include <random>

namespace vertex::tests {

namespace {
constexpr float kTwoPi = 6.28318530717958647692f;
}

std::vector<std::unique_ptr<BodyPhysics>> createStressTestBodies(const StressTestConfig& config) {
    std::vector<std::unique_ptr<BodyPhysics>> bodies;
    bodies.reserve(static_cast<std::size_t>(config.entityCount));

    std::mt19937 rng(config.seed);

    const float maxX = static_cast<float>(config.windowSize.width) - config.bodySize.x;
    const float maxY = static_cast<float>(config.windowSize.height) - config.bodySize.y;
    std::uniform_real_distribution<float> posX(0.0f, maxX > 0.0f ? maxX : 0.0f);
    std::uniform_real_distribution<float> posY(0.0f, maxY > 0.0f ? maxY : 0.0f);
    std::uniform_real_distribution<float> speed(config.minSpeed, config.maxSpeed);
    std::uniform_real_distribution<float> angle(0.0f, kTwoPi);

    for (int i = 0; i < config.entityCount; ++i) {
        auto body = std::make_unique<BodyPhysics>(glm::vec2{posX(rng), posY(rng)}, config.bodySize);

        const float a = angle(rng);
        const float s = speed(rng);
        body->setVelocity({s * std::cos(a), s * std::sin(a)});

        bodies.push_back(std::move(body));
    }

    return bodies;
}

void stepStressTest(std::vector<std::unique_ptr<BodyPhysics>>& bodies, float deltaTime,
                    const Dimensions& windowSize) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        for (std::size_t j = i + 1; j < bodies.size(); ++j) {
            bodies[i]->resolveCollision(*bodies[j]);
        }
    }

    for (auto& body : bodies) {
        body->update(deltaTime, windowSize);
    }
}

void drawStressTest(Renderer& renderer, Texture& tex,
                    const std::vector<std::unique_ptr<BodyPhysics>>& bodies) {
    for (const auto& body : bodies) {
        const glm::vec3 color = body->isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                       : glm::vec3(1.0f, 1.0f, 1.0f);
        renderer.drawSprite(tex, body->getPosition(), body->getSize(), {0, 0, 1, 1},
                            glm::vec4(color, 1.0f));
    }
}

}
