#pragma once

#include <cstdint>

namespace vertex {

struct Entity {
    std::uint32_t index{0};
    std::uint32_t generation{0};

    bool operator==(const Entity& other) const {
        return index == other.index && generation == other.generation;
    }

    bool operator!=(const Entity& other) const {
        return !(*this == other);
    }
};

}