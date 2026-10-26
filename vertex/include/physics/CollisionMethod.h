#pragma once

#include <memory>
#include <vector>

#include "physics/Physics.h"

namespace vertex::tests {

class CollisionMethod {
public:
    virtual ~CollisionMethod() = default;

    // name for cli parameter
    virtual const char* name() const = 0;

    // @brief collision resolution method called withing the game loop
    // this function then call the corresponding methods and manages the
    // logic for how the Data structures handles body collision
    virtual void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) = 0;
};

}