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
    float rotation{0.0f};   // radians
    float spinSpeed{0.0f};  // radians/sec
};

// Fixed-size pool of particles.
class ParticleEmitter {
public:
    explicit ParticleEmitter(std::size_t maxParticles = 500, float size = 10.0f,
                             float spawnInterval = 0.08f, float lifetime = 1.5f);

    void update(float dt, glm::vec2 pos, glm::vec2 vel, bool emitting,
                glm::vec2 offset = glm::vec2(0.0f));

    void draw(Renderer& renderer, Texture& tex) const;

    std::size_t aliveCount() const;

private:
    std::size_t firstUnused();
    void respawn(Particle& p, glm::vec2 pos, glm::vec2 vel, glm::vec2 offset);

    std::vector<Particle> mParticles;
    std::size_t mLastUsed{0};
    float mSize;
    float mSpawnInterval;
    float mLifetime;
    float mSpawnTimer{0.0f};
    std::mt19937 mRng{std::random_device{}()};
};

}
