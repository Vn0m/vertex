#pragma once

#include <glm/vec2.hpp>

namespace vertex{
    class BodyPhysics{
        public:
            BodyPhysics(const glm::vec2& position, const glm::vec2& size);
            ~BodyPhysics() = default;

            BodyPhysics(const BodyPhysics&) = delete;
            BodyPhysics& operator=(const BodyPhysics&) = delete;

            void update(float delta_time);
            void setVelocity(const glm::vec2& velocity);

            bool checkCollision(const BodyPhysics& other_entity) const;
            // bounce-back "animation"
            bool resolveCollision(BodyPhysics& other);

            glm::vec2 getPosition() const;
            glm::vec2 getSize() const;
        private:
            glm::vec2 m_position_;
            glm::vec2 m_size_;
            glm::vec2 m_velocity_{0.0f, 0.0f};
    };
}