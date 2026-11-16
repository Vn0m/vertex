#pragma once

#include <glm/vec2.hpp>
#include "vertex/types.h"
#include "vertex/Tilemap.h"

namespace vertex {

class Tilemap;

class BodyPhysics {
public:
    BodyPhysics(const glm::vec2& position, const glm::vec2& size);
    ~BodyPhysics() = default;

    BodyPhysics(const BodyPhysics&) = delete;
    BodyPhysics& operator=(const BodyPhysics&) = delete;

    // tilemap
    void update(float deltaTime, const Tilemap& map);

    // update to include window dimensions
    void update(float deltaTime, const Dimensions& windowSize);
    void setVelocity(const glm::vec2& velocity);

    bool checkCollision(const BodyPhysics& other) const;
    // bounce-back "animation"
    bool resolveCollision(BodyPhysics& other);

    glm::vec2 getPosition() const;
    glm::vec2 getSize() const;

    bool isKnockedBack() const {
        return mIsKnockback;
    }

private:
    void tickKnockback(float deltaTime);

    glm::vec2 mPosition;
    glm::vec2 mSize;
    glm::vec2 mVelocity{0.0f, 0.0f};

    glm::vec2 mOriginalVelocity{0.0f, 0.0f};
    float mKnockbackTimer{0.0f};
    bool mIsKnockback{false};
};

}
