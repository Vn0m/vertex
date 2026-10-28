#pragma once

#include <cstdint>
#include <vector>

#include "CollisionMethod.h"
#include "QuadTree.h"

namespace vertex {

class QuadTreeCollision : public CollisionMethod {
public:
    const char* name() const override {
        return "quadtree";
    }

    void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) override;

private:
    static constexpr float kMinNodeSizeFactor = 1.0f;

    // Kept between frames so tree nodes and the neighbor buffer reuse their memory.
    QuadTree mTree;
    std::vector<std::uint32_t> mNeighbors;
};

}
