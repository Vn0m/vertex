#include "vertex/Physics.h"

#include "glm/glm.hpp"

namespace vertex{

BodyPhysics::BodyPhysics(const glm::vec2& position, const glm::vec2& size) : m_position_(position), m_size_(size) {}

// updated to account for window dimensions as rigid body (for now)
void BodyPhysics::update(float delta_time, const Dimensions& windowSize){
    // handle knockback
    if(mIsKnockback){
        mKnockbackTimer -= delta_time;
        if(mKnockbackTimer <= 0.0f){
            mIsKnockback = false;
            m_velocity_ = mOriginalVelocity;
        }
    }
    
    // velocity application
    m_position_ += m_velocity_ * delta_time;


    float halfWidth = m_size_.x / 2.0f;
    float halfHeight = m_size_.y / 2.0f;

    // window dimensison
    float winWidth = static_cast<float>(windowSize.width);
    float winHeight = static_cast<float>(windowSize.height);

    // left wall
    if(m_position_.x - halfWidth <= 0.0f){
        m_position_.x = halfWidth;
        m_velocity_.x *= -1.0f;
    }
    // right wall
    else if(m_position_.x + halfWidth >= winWidth){
        m_position_.x = winWidth - halfWidth;
        m_velocity_.x *= -1.0f;
    }

    // top wall 
    if(m_position_.y - halfHeight <= 0.0f){
        m_position_.y = halfHeight;
        m_velocity_.y *= -1.0f;
    }
    // bottom wall
    else if(m_position_.y + halfHeight >= winHeight){
        m_position_.y = winHeight - halfHeight;
        m_velocity_.y *= -1.0f;
    }
}

void BodyPhysics::setVelocity(const glm::vec2& velocity){
    if(!mIsKnockback) m_velocity_ = velocity;
}

bool BodyPhysics::checkCollision(const BodyPhysics& other_entity) const{
    bool collision_x = this -> m_position_.x + m_size_.x > other_entity.m_position_.x && 
                        other_entity.m_position_.x + other_entity.m_size_.x > this -> m_position_.x;

    bool collision_y = this -> m_position_.y + m_size_.y > other_entity.m_position_.y && 
                        other_entity.m_position_.y + other_entity.m_size_.y > this -> m_position_.y;

    return collision_x && collision_y;
}

bool BodyPhysics::resolveCollision(BodyPhysics& other_entity){
    if(checkCollision(other_entity)){

        float knockbackSpeed = 150.0f;
        float knowckbackDuration = 0.25f;

        glm::vec2 centerA = m_position_ + (m_size_ * 0.5f);
        glm::vec2 centerB = other_entity.m_position_ + (other_entity.m_size_ * 0.5f);

        glm::vec2 pushDirA = glm::normalize(centerA - centerB);
        glm::vec2 pushDirB = -pushDirA;

        if(!(this -> mIsKnockback)){
            this -> mOriginalVelocity = this -> m_velocity_;
            this -> m_velocity_ = pushDirA * knockbackSpeed;
            this -> mKnockbackTimer = knowckbackDuration;
            this -> mIsKnockback = true;
        }
        
        if(other_entity.mIsKnockback){
            other_entity.mOriginalVelocity = other_entity.m_velocity_;
            other_entity.m_velocity_ = pushDirA * knockbackSpeed;
            other_entity.mKnockbackTimer = knowckbackDuration;
            other_entity.mIsKnockback = true;
        }
        return true;
    }

    return false;
}

glm::vec2 BodyPhysics::getPosition() const { return m_position_; }
glm::vec2 BodyPhysics::getSize() const { return m_size_; }
}