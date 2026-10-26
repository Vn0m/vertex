#pragma once

// include virtual class
#include "CollisionMethod.h"

namespace vertex::tests {

class BruteForceCollision : public CollisionMethod {
public:
    const char* name() const override {
        return "brute";
    }

    // @param takes a vector containing POINTERS to all physics objects.
    void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) override;
};

}
