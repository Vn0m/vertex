#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace vertex {

struct Camera {
    glm::vec2 position{0.0f};
    glm::vec2 viewportSize{1280.0f, 720.0f};
    float zoom{1.0f};

    glm::mat4 viewProjection() const {
        const glm::vec2 half = viewportSize / zoom * 0.5f;
        return glm::ortho(position.x - half.x, position.x + half.x, position.y + half.y,
                          position.y - half.y, -1.0f, 1.0f);
    }
};

}
