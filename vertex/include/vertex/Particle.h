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

struct EmitterSettings {
    std::size_t maxParticles{500};
    float size{10.0f};
    float spawnInterval{0.08f};
    float lifetime{1.5f};
    float jitter{0.0f};
    float inheritVelocity{0.0f};
    float spread{0.0f};
    float spin{0.0f};
    float minBrightness{1.0f};
};

class ParticleEmitter {
public:
    explicit ParticleEmitter(const EmitterSettings& settings);

    void update(float dt, glm::vec2 pos, glm::vec2 vel, bool emitting);

    void draw(Renderer& renderer, Texture& tex) const;

    std::size_t aliveCount() const;

private:
    std::size_t firstUnused();
    void respawn(Particle& p, glm::vec2 pos, glm::vec2 vel);

    EmitterSettings mSettings;
    std::vector<Particle> mParticles;
    std::size_t mLastUsed{0};
    float mSpawnTimer{0.0f};
    std::mt19937 mRng{std::random_device{}()};
};

}
