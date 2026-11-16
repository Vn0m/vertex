#include <glm/gtc/constants.hpp>
#include "vertex/Particle.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"

namespace vertex {

ParticleEmitter::ParticleEmitter(const EmitterSettings& settings)
    : mSettings(settings), mParticles(settings.maxParticles) {}

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

void ParticleEmitter::respawn(Particle& p, glm::vec2 pos, glm::vec2 vel) {
    auto rand = [this](float lo, float hi) {
        return std::uniform_real_distribution<float>(lo, hi)(mRng);
    };
    auto randVec = [&](float range) {
        return glm::vec2(rand(-range, range), rand(-range, range));
    };

    const float brightness = rand(mSettings.minBrightness, 1.0f);
    p.position = pos + randVec(mSettings.jitter);
    p.color = glm::vec4(brightness, brightness, brightness, 1.0f);
    p.life = mSettings.lifetime;
    p.velocity = vel * mSettings.inheritVelocity + randVec(mSettings.spread);
    p.rotation = rand(0.0f, glm::two_pi<float>());
    p.spinSpeed = rand(-mSettings.spin, mSettings.spin);
}

void ParticleEmitter::update(float dt, glm::vec2 pos, glm::vec2 vel, bool emitting) {
    if (emitting) {
        mSpawnTimer += dt;
        while (mSpawnTimer >= mSettings.spawnInterval) {
            mSpawnTimer -= mSettings.spawnInterval;
            respawn(mParticles[firstUnused()], pos, vel);
        }
    } else {
        mSpawnTimer = 0.0f;
    }

    for (Particle& p : mParticles) {
        if (p.life <= 0.0f) continue;

        p.life -= dt;
        p.position += p.velocity * dt;
        p.rotation += p.spinSpeed * dt;
        p.color.a = p.life / mSettings.lifetime;
    }
}

void ParticleEmitter::draw(Renderer& renderer, Texture& tex) const {
    for (const Particle& p : mParticles) {
        if (p.life > 0.0f) {
            renderer.drawSprite(tex, p.position, {mSettings.size, mSettings.size},
                                {0, 0, 1, 1}, p.color, p.rotation);
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
