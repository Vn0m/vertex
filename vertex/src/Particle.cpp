#include "vertex/Particle.h"

#include "vertex/Renderer.h"
#include "vertex/Texture.h"

namespace vertex {

ParticleEmitter::ParticleEmitter(std::size_t maxParticles, float size)
    : mParticles(maxParticles), mSize(size) {}

std::size_t ParticleEmitter::firstUnused() {
    // search from the last used slot; the next dead one is usually right after it
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
    std::uniform_real_distribution<float> spread(-8.0f, 8.0f);
    std::uniform_real_distribution<float> shade(0.7f, 1.0f);
    std::uniform_real_distribution<float> drift(-20.0f, 20.0f);

    const float s = shade(mRng);
    p.position = pos + offset + glm::vec2(spread(mRng), spread(mRng));
    p.color = glm::vec4(s, s, s, 1.0f);
    p.life = 1.0f;
    // trail behind the mover with a little sideways flutter
    p.velocity = vel * 0.1f + glm::vec2(drift(mRng), drift(mRng));
}

void ParticleEmitter::update(float dt, glm::vec2 pos, glm::vec2 vel, int spawnCount,
                             glm::vec2 offset) {
    for (int i = 0; i < spawnCount; ++i) {
        respawn(mParticles[firstUnused()], pos, vel, offset);
    }

    for (Particle& p : mParticles) {
        p.life -= dt;
        if (p.life > 0.0f) {
            p.position -= p.velocity * dt;
            p.color.a -= dt * 1.0f;
        }
    }
}

void ParticleEmitter::draw(Renderer& renderer, Texture& tex) const {
    for (const Particle& p : mParticles) {
        if (p.life > 0.0f) {
            renderer.drawSprite(tex, p.position, {mSize, mSize}, {0, 0, 1, 1}, p.color);
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
