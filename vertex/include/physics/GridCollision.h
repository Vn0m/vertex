#pragma once

#include <cstdint>
#include <vector>

#include "physics/CollisionMethod.h"
#include "physics/SpatialGrid.h"

namespace vertex::tests {

class GridCollision : public CollisionMethod {
public:
    const char* name() const override {
        return "grid";
    }

    void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) override;

private:
    static constexpr float kCellSizeFactor = 2.0f;

    SpatialGrid mGrid;
    // container with nearby physic bodies ids
    std::vector<std::uint32_t> mNeighbors;
};

}
