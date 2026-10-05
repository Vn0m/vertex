#include "vertex/Physics.h"

#include "glm/glm.hpp"

namespace vertex {

BodyPhysics::BodyPhysics(const glm::vec2& position, const glm::vec2& size)
    : mPosition(position), mSize(size) {}

// updated to account for window dimensions as rigid body (for now)
void BodyPhysics::update(float deltaTime, const Dimensions& windowSize) {
    // handle knockback
    if (mIsKnockback) {
        mKnockbackTimer -= deltaTime;
        if (mKnockbackTimer <= 0.0f) {
            mIsKnockback = false;
            mVelocity = mOriginalVelocity;
        }
    }

    // velocity application
    mPosition += mVelocity * deltaTime;

    // window dimensions
    float winWidth = static_cast<float>(windowSize.width);
    float winHeight = static_cast<float>(windowSize.height);

    // left wall
    if (mPosition.x <= 0.0f) {
        mPosition.x = 0.0f;
        mVelocity.x *= -1.0f;
    }
    // right wall
    else if (mPosition.x + mSize.x >= winWidth) {
        mPosition.x = winWidth - mSize.x;
        mVelocity.x *= -1.0f;
    }

    // top wall
    if (mPosition.y <= 0.0f) {
        mPosition.y = 0.0f;
        mVelocity.y *= -1.0f;
    }
    // bottom wall
    else if (mPosition.y + mSize.y >= winHeight) {
        mPosition.y = winHeight - mSize.y;
        mVelocity.y *= -1.0f;
    }
}

void BodyPhysics::setVelocity(const glm::vec2& velocity) {
    if (!mIsKnockback) mVelocity = velocity;
}

void BodyPhysics::setPosition(const glm::vec2& position) {
    mPosition = position;
}

bool BodyPhysics::checkCollision(const BodyPhysics& other) const {
    bool collisionX = mPosition.x + mSize.x > other.mPosition.x &&
                      other.mPosition.x + other.mSize.x > mPosition.x;

    bool collisionY = mPosition.y + mSize.y > other.mPosition.y &&
                      other.mPosition.y + other.mSize.y > mPosition.y;

    return collisionX && collisionY;
}

bool BodyPhysics::resolveCollision(BodyPhysics& other) {
    if (checkCollision(other)) {
        float knockbackSpeed = 150.0f;
        float knockbackDuration = 0.25f;

        glm::vec2 centerA = mPosition + (mSize * 0.5f);
        glm::vec2 centerB = other.mPosition + (other.mSize * 0.5f);

        glm::vec2 pushDirA = glm::normalize(centerA - centerB);
        glm::vec2 pushDirB = -pushDirA;

        if (!mIsKnockback) {
            mOriginalVelocity = mVelocity;
            mVelocity = pushDirA * knockbackSpeed;
            mKnockbackTimer = knockbackDuration;
            mIsKnockback = true;
        }

        if (!other.mIsKnockback) {
            other.mOriginalVelocity = other.mVelocity;
            other.mVelocity = pushDirB * knockbackSpeed;
            other.mKnockbackTimer = knockbackDuration;
            other.mIsKnockback = true;
        }
        return true;
    }

    return false;
}

glm::vec2 BodyPhysics::getPosition() const {
    return mPosition;
}
glm::vec2 BodyPhysics::getSize() const {
    return mSize;
}

glm::vec2 BodyPhysics::getVelocity() const {
    return mVelocity;
}

}
