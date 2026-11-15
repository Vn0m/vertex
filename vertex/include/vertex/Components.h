#pragma once

#include <glm/glm.hpp>

namespace vertex {

class Texture;

struct Transform {
    glm::vec2 position{0.0f};
};

struct Velocity {
    glm::vec2 value{0.0f};
};

struct Sprite {
    Texture* texture{nullptr};
    glm::vec4 uvRect{0.0f, 0.0f, 1.0f, 1.0f};
    glm::vec2 size{0.0f};
    glm::vec4 tint{1.0f};
};

struct Collider {
    glm::vec2 offset{0.0f};
    glm::vec2 size{0.0f};
};

}
