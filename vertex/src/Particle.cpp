#include <glm/gtc/constants.hpp>
#include "vertex/Particle.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"

namespace vertex {

ParticleEmitter::ParticleEmitter(std::size_t maxParticles, float size,
                                 float spawnInterval, float lifetime)
    : mParticles(maxParticles),
      mSize(size),
      mSpawnInterval(spawnInterval),
      mLifetime(lifetime) {}

std::size_t ParticleEmitter::firstUnused() {
    for (std::size_t i = mLastUsed; i < mParticles.size(); ++i) {
        if (mParticles[i].life <= 0.0f) {
            mLastUsed = i;
            return i;
        }
    }
    for (std::size_t i = 0; i < mLastUsed; ++i) {
        if (mParticles[i].life <= 0.0f) {
            mLastUsed = i;
            return i;
        }
    }
    // pool is full: overwrite the first particle
    mLastUsed = 0;
    return 0;
}

void ParticleEmitter::respawn(Particle& p, glm::vec2 pos, glm::vec2 vel,
                              glm::vec2 offset) {
    auto rand = [this](float lo, float hi) {
        return std::uniform_real_distribution<float>(lo, hi)(mRng);
    };

    const float s = rand(0.7f, 1.0f);
    p.position = pos + offset + glm::vec2(rand(-8.0f, 8.0f), rand(-8.0f, 8.0f));
    p.color = glm::vec4(s, s, s, 1.0f);
    p.life = mLifetime;
    p.velocity = -vel * 0.1f + glm::vec2(rand(-20.0f, 20.0f), rand(-20.0f, 20.0f));
    p.rotation = rand(0.0f, glm::two_pi<float>());
    p.spinSpeed = rand(-3.0f, 3.0f);
}

void ParticleEmitter::update(float dt, glm::vec2 pos, glm::vec2 vel, bool emitting,
                             glm::vec2 offset) {
    if (emitting) {
        mSpawnTimer += dt;
        while (mSpawnTimer >= mSpawnInterval) {
            mSpawnTimer -= mSpawnInterval;
            respawn(mParticles[firstUnused()], pos, vel, offset);
        }
    } else {
        mSpawnTimer = 0.0f;
    }

    for (Particle& p : mParticles) {
        if (p.life <= 0.0f) continue;

        p.life -= dt;
        p.position += p.velocity * dt;
        p.rotation += p.spinSpeed * dt;
        p.color.a = p.life / mLifetime;
    }
}

void ParticleEmitter::draw(Renderer& renderer, Texture& tex) const {
    for (const Particle& p : mParticles) {
        if (p.life > 0.0f) {
            renderer.drawSprite(tex, p.position, {mSize, mSize}, {0, 0, 1, 1}, p.color,
                                p.rotation);
        }
    }
}

std::size_t ParticleEmitter::aliveCount() const {
    std::size_t n = 0;
    for (const Particle& p : mParticles) {
        if (p.life > 0.0f) ++n;
    }
    return n;
}

}
