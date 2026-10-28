#pragma once

#include "CollisionMethod.h"

namespace vertex {

class BruteForceCollision : public CollisionMethod {
public:
    const char* name() const override {
        return "brute";
    }

    void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) override;
};

}
