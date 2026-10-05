#include "PhysicsTilemapTest.h"

#include <algorithm>
#include <cmath>
#include <random>

namespace vertex::tests {

namespace {
constexpr float kTwoPi = 6.28318530717958647692f;

// Pushes `body` fully outside `wall` along whichever axis has the smaller
// overlap, and flips the matching velocity component -- mirroring the
// wall-bounce convention BodyPhysics::update already uses for the window
// edges. The actual position correction (not just a velocity flip) is what
// keeps bodies from fluttering in place when they're still overlapping on
// the next frame.
void resolveWallCollision(BodyPhysics& body, const Wall& wall) {
    glm::vec2 pos = body.getPosition();
    const glm::vec2 size = body.getSize();

    const bool overlapsX = pos.x + size.x > wall.position.x &&
                           wall.position.x + wall.size.x > pos.x;
    const bool overlapsY = pos.y + size.y > wall.position.y &&
                           wall.position.y + wall.size.y > pos.y;
    if (!overlapsX || !overlapsY) {
        return;
    }

    const float overlapX = std::min(pos.x + size.x, wall.position.x + wall.size.x) -
                           std::max(pos.x, wall.position.x);
    const float overlapY = std::min(pos.y + size.y, wall.position.y + wall.size.y) -
                           std::max(pos.y, wall.position.y);

    glm::vec2 velocity = body.getVelocity();
    const glm::vec2 bodyCenter = pos + size * 0.5f;
    const glm::vec2 wallCenter = wall.position + wall.size * 0.5f;

    if (overlapX < overlapY) {
        pos.x += (bodyCenter.x < wallCenter.x) ? -overlapX : overlapX;
        velocity.x = -velocity.x;
    } else {
        pos.y += (bodyCenter.y < wallCenter.y) ? -overlapY : overlapY;
        velocity.y = -velocity.y;
    }

    body.setPosition(pos);
    body.setVelocity(velocity);
}

}

Dimensions windowSizeForTilemapTest(const TilemapTestConfig& config) {
    return Dimensions{config.gridCols * config.tileSize, config.gridRows * config.tileSize};
}

std::vector<Wall> buildBorderWalls(const TilemapTestConfig& config) {
    const float tile = static_cast<float>(config.tileSize);
    const float width = static_cast<float>(config.gridCols) * tile;
    const float height = static_cast<float>(config.gridRows) * tile;

    std::vector<Wall> walls;
    walls.push_back({{0.0f, 0.0f}, {width, tile}});                    // top
    walls.push_back({{0.0f, height - tile}, {width, tile}});           // bottom
    walls.push_back({{0.0f, 0.0f}, {tile, height}});                  // left
    walls.push_back({{width - tile, 0.0f}, {tile, height}});          // right
    return walls;
}

std::vector<std::unique_ptr<BodyPhysics>> createTilemapTestBodies(const TilemapTestConfig& config) {
    std::vector<std::unique_ptr<BodyPhysics>> bodies;
    bodies.reserve(static_cast<std::size_t>(config.entityCount));

    std::mt19937 rng(config.seed);

    const float tile = static_cast<float>(config.tileSize);
    const float minX = tile;
    const float minY = tile;
    const float maxX = static_cast<float>(config.gridCols - 1) * tile - config.bodySize.x;
    const float maxY = static_cast<float>(config.gridRows - 1) * tile - config.bodySize.y;

    std::uniform_real_distribution<float> posX(minX, std::max(minX, maxX));
    std::uniform_real_distribution<float> posY(minY, std::max(minY, maxY));
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

void stepTilemapTest(std::vector<std::unique_ptr<BodyPhysics>>& bodies,
                     const std::vector<Wall>& walls, float deltaTime,
                     const Dimensions& windowSize) {
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        for (std::size_t j = i + 1; j < bodies.size(); ++j) {
            bodies[i]->resolveCollision(*bodies[j]);
        }
    }

    for (auto& body : bodies) {
        for (const Wall& wall : walls) {
            resolveWallCollision(*body, wall);
        }
        body->update(deltaTime, windowSize);
    }
}

void drawTilemapTest(Renderer& renderer, Texture& backgroundTex, Texture& bodyTex,
                     const Dimensions& windowSize,
                     const std::vector<std::unique_ptr<BodyPhysics>>& bodies) {
    renderer.drawSprite(backgroundTex, {0.0f, 0.0f},
                        {static_cast<float>(windowSize.width), static_cast<float>(windowSize.height)},
                        {0, 0, 1, 1});

    for (const auto& body : bodies) {
        const glm::vec3 color = body->isKnockedBack() ? glm::vec3(1.0f, 0.0f, 0.0f)
                                                       : glm::vec3(1.0f, 1.0f, 1.0f);
        renderer.drawSprite(bodyTex, body->getPosition(), body->getSize(), {0, 0, 1, 1},
                            glm::vec4(color, 1.0f));
    }
}

}
