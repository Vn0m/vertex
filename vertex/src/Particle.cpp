#include "vertex/Particle.h"
#include "vertex/Renderer.h"
#include "vertex/Texture.h"

#include <cmath>

namespace vertex {

ParticleEmitter::ParticleEmitter(std::size_t maxParticles, float size,
                                 ParticleConfig config)
    : mParticles(maxParticles), mSize(size), mConfig(config) {}

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
    mLastUsed = 0;
    return 0;
}

void ParticleEmitter::respawn(Particle& p, glm::vec2 pos, glm::vec2 vel,
                              glm::vec2 offset) {
    auto rand = [this](float lo, float hi) {
        return std::uniform_real_distribution<float>(lo, hi)(mRng);
    };

    const bool moving = glm::length(vel) > 0.0f;
    const float behind = moving ? std::atan2(-vel.y, -vel.x) : 0.0f;
    const float spread = moving ? mConfig.spread : 3.14159265f;

    const float shade = rand(0.7f, 1.0f);
    p = Particle{};
    p.position = pos + offset +
                 glm::vec2(rand(-mConfig.spawnJitter, mConfig.spawnJitter),
                           rand(-mConfig.spawnJitter, mConfig.spawnJitter));
    p.color = glm::vec4(shade, shade, shade, 1.0f);
    p.life = mConfig.lifetime;
    p.baseAngle = behind + rand(-spread, spread);
    p.baseSpeed = rand(mConfig.minSpeed, mConfig.maxSpeed);
    p.spinSpeed = rand(-mConfig.maxSpin, mConfig.maxSpin);
    p.swayPhase = rand(0.0f, 6.2831853f);
    p.swayAmount = rand(0.0f, mConfig.maxSway);
    p.rotation = rand(0.0f, 6.2831853f);
}

void ParticleEmitter::update(float dt, glm::vec2 pos, glm::vec2 vel, bool emitting,
                             glm::vec2 offset) {
    if (emitting) {
        mSpawnTimer += dt;
        while (mSpawnTimer >= mConfig.spawnInterval) {
            mSpawnTimer -= mConfig.spawnInterval;
            respawn(mParticles[firstUnused()], pos, vel, offset);
        }
    } else {
        mSpawnTimer = 0.0f;
    }

    for (Particle& p : mParticles) {
        if (p.life <= 0.0f) continue;

        p.life -= dt;
        p.age += dt;
        p.rotation += p.spinSpeed * dt;

        // Sway back and forth relative to base direction
        const float currentAngle =
            p.baseAngle +
            std::sin(p.age * mConfig.swayFrequency + p.swayPhase) * p.swayAmount;
        const glm::vec2 dir{std::cos(currentAngle), std::sin(currentAngle)};

        p.position += dir * p.baseSpeed * dt;
        p.color.a = p.life / mConfig.lifetime;
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
