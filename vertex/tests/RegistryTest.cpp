#include <vertex/Registry.h>

#include <cstdio>
#include <vector>

#include "Check.h"

namespace {

struct Position {
    float x{0};
    float y{0};
};

struct Velocity {
    float dx{0};
    float dy{0};
};

void createsDistinctEntities() {
    vertex::Registry registry;

    const vertex::Entity a = registry.create();
    const vertex::Entity b = registry.create();
    const vertex::Entity c = registry.create();

    CHECK(registry.valid(a) && registry.valid(b) && registry.valid(c));
    CHECK(a != b && b != c && a != c);
    CHECK(registry.aliveCount() == 3);
}

void destroyInvalidatesTheHandle() {
    vertex::Registry registry;

    const vertex::Entity a = registry.create();
    registry.destroy(a);

    CHECK(!registry.valid(a));
    CHECK(registry.aliveCount() == 0);

    registry.destroy(a);
    CHECK(registry.aliveCount() == 0);
}

void recycledSlotRejectsTheOldHandle() {
    vertex::Registry registry;

    const vertex::Entity first = registry.create();
    registry.destroy(first);

    const vertex::Entity second = registry.create();

    CHECK(second.index == first.index);
    CHECK(second.generation == first.generation + 1);
    CHECK(registry.valid(second));
    CHECK(!registry.valid(first));
}

void componentRoundTrip() {
    vertex::Registry registry;
    const vertex::Entity e = registry.create();

    CHECK(!registry.has<Position>(e));
    CHECK(registry.get<Position>(e) == nullptr);

    registry.add(e, Position{3, 4});

    CHECK(registry.has<Position>(e));
    CHECK(registry.get<Position>(e)->x == 3);

    registry.get<Position>(e)->x = 9;
    CHECK(registry.get<Position>(e)->x == 9);

    registry.remove<Position>(e);
    CHECK(!registry.has<Position>(e));
    CHECK(registry.get<Position>(e) == nullptr);
}

void destroyClearsEveryPool() {
    vertex::Registry registry;

    const vertex::Entity a = registry.create();
    const vertex::Entity b = registry.create();

    registry.add(a, Position{1, 1});
    registry.add(a, Velocity{1, 1});
    registry.add(b, Position{2, 2});

    registry.destroy(a);

    CHECK(!registry.has<Position>(a));
    CHECK(!registry.has<Velocity>(a));
    CHECK(registry.has<Position>(b));
    CHECK(registry.get<Position>(b)->x == 2);
}

void deadHandlesReadAsEmpty() {
    vertex::Registry registry;

    const vertex::Entity a = registry.create();
    registry.add(a, Position{1, 1});
    registry.destroy(a);

    CHECK(!registry.has<Position>(a));
    CHECK(registry.get<Position>(a) == nullptr);

    registry.remove<Position>(a);
}

void eachOneComponent() {
    vertex::Registry registry;

    const vertex::Entity a = registry.create();
    const vertex::Entity b = registry.create();
    registry.create();

    registry.add(a, Position{1, 0});
    registry.add(b, Position{2, 0});

    std::vector<std::uint32_t> seen;
    registry.each<Position>(
        [&](vertex::Entity e, Position&) { seen.push_back(e.index); });

    CHECK(seen.size() == 2);
    CHECK((seen[0] == a.index && seen[1] == b.index) ||
          (seen[0] == b.index && seen[1] == a.index));
}

void eachTwoComponentsMatchesIntersection() {
    vertex::Registry registry;

    const vertex::Entity both = registry.create();
    const vertex::Entity onlyPosition = registry.create();
    const vertex::Entity onlyVelocity = registry.create();

    registry.add(both, Position{1, 1});
    registry.add(both, Velocity{5, 5});
    registry.add(onlyPosition, Position{2, 2});
    registry.add(onlyVelocity, Velocity{9, 9});

    int visits = 0;
    registry.each<Position, Velocity>([&](vertex::Entity e, Position& p, Velocity& v) {
        CHECK(e == both);
        CHECK(p.x == 1);
        CHECK(v.dx == 5);
        visits++;
    });

    CHECK(visits == 1);
}

void eachPicksTheSmallerPoolEitherWay() {
    vertex::Registry registry;
    std::vector<vertex::Entity> entities;

    for (int i = 0; i < 100; ++i) {
        entities.push_back(registry.create());
    }

    for (const vertex::Entity e : entities) {
        registry.add(e, Position{0, 0});
    }
    registry.add(entities[7], Velocity{1, 1});
    registry.add(entities[42], Velocity{2, 2});

    int visits = 0;
    registry.each<Position, Velocity>(
        [&](vertex::Entity, Position&, Velocity&) { visits++; });
    CHECK(visits == 2);

    visits = 0;
    registry.each<Velocity, Position>(
        [&](vertex::Entity, Velocity&, Position&) { visits++; });
    CHECK(visits == 2);
}

void componentsSurviveNeighbourDestruction() {
    vertex::Registry registry;
    std::vector<vertex::Entity> entities;

    for (int i = 0; i < 100; ++i) {
        const vertex::Entity e = registry.create();
        registry.add(e, Position{static_cast<float>(i), 0});
        entities.push_back(e);
    }

    for (std::size_t i = 0; i < entities.size(); i += 2) {
        registry.destroy(entities[i]);
    }

    for (std::size_t i = 0; i < entities.size(); ++i) {
        if (i % 2 == 0) {
            CHECK(!registry.valid(entities[i]));
            CHECK(!registry.has<Position>(entities[i]));
        } else {
            CHECK(registry.valid(entities[i]));
            CHECK(registry.get<Position>(entities[i])->x == static_cast<float>(i));
        }
    }
}

void tenThousandEntitiesRejectEveryStaleHandle() {
    vertex::Registry registry;
    std::vector<vertex::Entity> entities;

    for (int i = 0; i < 10000; ++i) {
        const vertex::Entity e = registry.create();
        registry.add(e, Position{static_cast<float>(i), 0});
        entities.push_back(e);
    }
    CHECK(registry.aliveCount() == 10000);

    std::vector<vertex::Entity> destroyed;
    for (std::size_t i = 0; i < entities.size(); i += 2) {
        registry.destroy(entities[i]);
        destroyed.push_back(entities[i]);
    }
    CHECK(registry.aliveCount() == 5000);

    std::vector<vertex::Entity> recycled;
    for (int i = 0; i < 5000; ++i) {
        recycled.push_back(registry.create());
    }
    CHECK(registry.aliveCount() == 10000);

    for (const vertex::Entity e : destroyed) {
        CHECK(!registry.valid(e));
        CHECK(!registry.has<Position>(e));
        CHECK(registry.get<Position>(e) == nullptr);
    }

    for (const vertex::Entity e : recycled) {
        CHECK(registry.valid(e));
    }

    for (std::size_t i = 1; i < entities.size(); i += 2) {
        CHECK(registry.valid(entities[i]));
        CHECK(registry.get<Position>(entities[i])->x == static_cast<float>(i));
    }
}

}

int main() {
    createsDistinctEntities();
    destroyInvalidatesTheHandle();
    recycledSlotRejectsTheOldHandle();
    componentRoundTrip();
    destroyClearsEveryPool();
    deadHandlesReadAsEmpty();
    eachOneComponent();
    eachTwoComponentsMatchesIntersection();
    eachPicksTheSmallerPoolEitherWay();
    componentsSurviveNeighbourDestruction();
    tenThousandEntitiesRejectEveryStaleHandle();

    std::printf("Registry: all tests passed\n");
    return 0;
}
