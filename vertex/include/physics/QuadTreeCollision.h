#pragma once

#include <cstdint>
#include <vector>

#include "physics/CollisionMethod.h"
#include "physics/QuadTree.h"

namespace vertex::tests {

class QuadTreeCollision : public CollisionMethod {
public:
    const char* name() const override {
        return "quadtree";
    }

    void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) override;

private:
    // Smallest node side (like grid); 1.0 measured ~25% faster than 2.0
    static constexpr float kMinNodeSizeFactor = 1.0f;

    // Kept between frames so tree nodes and the neighbor buffer are reused.
    QuadTree mTree;

    // ID in a node
    std::vector<std::uint32_t> mNeighbors;
};

}
