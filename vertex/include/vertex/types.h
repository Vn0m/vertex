#pragma once

#include <cstdint>
#include <glm/glm.hpp>

namespace vertex {

struct Dimensions {
    std::int32_t width{640};
    std::int32_t height{480};
};

struct Rect {
    glm::vec2 position{0.0f};
    glm::vec2 size{0.0f};
};

}
