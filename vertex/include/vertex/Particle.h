#pragma once

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
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

    float age{0.0f};         // seconds since spawn, drives the sway
    float rotation{0.0f};    // radians
    float spinSpeed{0.0f};   // radians/sec, random sign and magnitude
    float baseAngle{0.0f};   // initial travel direction, radians
    float baseSpeed{0.0f};   // initial speed, pixels/sec
    float swayPhase{0.0f};   // random offset so petals don't sway in sync
    float swayAmount{0.0f};  // how far the direction wobbles, radians
};

// Tunable ranges; each particle picks random values inside them at spawn.
struct ParticleConfig {
    float spawnInterval{0.08f};  // seconds between petals while emitting
    float lifetime{1.5f};
    float spread{1.57f};  // +/- radians around "behind the emitter"
    float minSpeed{20.0f}, maxSpeed{60.0f};
    float maxSpin{3.0f};        // radians/sec, either direction
    float maxSway{0.9f};        // radians
    float swayFrequency{4.0f};  // radians/sec of the sine wave
    float spawnJitter{8.0f};    // pixels
};

// Fixed-size pool of particles.
// Not a Renderer: it only calls Renderer::drawSprite
class ParticleEmitter {
public:
    explicit ParticleEmitter(std::size_t maxParticles = 500, float size = 10.0f,
                             ParticleConfig config = {});

    // While emitting, spawns a particle every config.spawnInterval seconds (timer based,
    // so frame rate doesn't matter), then ages and moves the live ones.
    // vel is the emitter's velocity; particles are thrown out behind it.
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
    ParticleConfig mConfig;
    float mSpawnTimer{0.0f};
    std::mt19937 mRng{std::random_device{}()};
};

}
