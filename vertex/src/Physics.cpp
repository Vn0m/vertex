#include "vertex/Physics.h"

namespace vertex{

BodyPhysics::BodyPhysics(const glm::vec2& position, const glm::vec2& size) : m_position_(position), m_size_(size) {}

void BodyPhysics::update(float delta_time){
    m_position_ += m_velocity_ * delta_time;
}

void BodyPhysics::setVelocity(const glm::vec2& velocity){
    m_velocity_ = velocity;
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
        // for now it just makes them change direction
        this -> m_velocity_ = -(this -> m_velocity_);
        other_entity.m_velocity_ = -other_entity.m_velocity_;
        
        return true;
    }

    return false;
}

glm::vec2 BodyPhysics::getPosition() const { return m_position_; }
glm::vec2 BodyPhysics::getSize() const { return m_size_; }
}