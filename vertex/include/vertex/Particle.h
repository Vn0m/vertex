#pragma once

#include <glm/glm.hpp>
#include <cstddef>
#include <random>
#include <vector>

namespace vertex {

class Renderer;
class Texture;

struct Particle {
    glm::vec2 position{0.0f};
    glm::vec2 velocity{0.0f};
    glm::vec4 color{1.0f};
    float life{0.0f};
};

// Fixed-size pool of particles.
// Not a Renderer: it only calls Renderer::drawSprite
class ParticleEmitter {
public:
    explicit ParticleEmitter(std::size_t maxParticles = 500, float size = 10.0f);

    // Spawns spawnCount particles around pos, then ages and moves the live ones.
    // vel is the emitter's velocity; particles trail behind it.
    void update(float dt, glm::vec2 pos, glm::vec2 vel, int spawnCount,
                glm::vec2 offset = glm::vec2(0.0f));
    void draw(Renderer& renderer, Texture& tex) const;

    std::size_t aliveCount() const;

private:
    std::size_t firstUnused();
    void respawn(Particle& p, glm::vec2 pos, glm::vec2 vel, glm::vec2 offset);

    std::vector<Particle> mParticles;
    std::size_t mLastUsed{0};
    float mSize;
    std::mt19937 mRng{std::random_device{}()};
};

}
