#pragma once

#include <memory>
#include <vector>

#include "vertex/Physics.h"

namespace vertex::tests {

class CollisionMethod {
public:
    virtual ~CollisionMethod() = default;

    // name for cli parameter
    virtual const char* name() const = 0;

    // collision resolution method
    virtual void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) = 0;
};

}
