#pragma once

#include <memory>
#include <vector>

#include "Physics.h"

namespace vertex {

class CollisionMethod {
public:
    virtual ~CollisionMethod() = default;

    virtual const char* name() const = 0;

    virtual void resolve(std::vector<std::unique_ptr<BodyPhysics>>& bodies) = 0;
};

}
