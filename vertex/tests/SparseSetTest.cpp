#include <vertex/SparseSet.h>

#include "Check.h"
#include <cstdio>
#include <string>

namespace {

struct Position {
    float x{0};
    float y{0};
};

void addAndGet() {
    vertex::SparseSet<Position> set;

    set.add(1, {10, 11});
    set.add(2, {20, 22});
    set.add(3, {30, 33});

    CHECK(set.size() == 3);
    CHECK(set.has(1) && set.has(2) && set.has(3));

    CHECK(set.get(1)->x == 10);
    CHECK(set.get(2)->x == 20);
    CHECK(set.get(3)->x == 30);
}

void missingEntity() {
    vertex::SparseSet<Position> set;
    set.add(1, {10, 11});

    CHECK(!set.has(0));
    CHECK(!set.has(99));
    CHECK(set.get(0) == nullptr);
    CHECK(set.get(99) == nullptr);
}

void removeFromMiddle() {
    vertex::SparseSet<Position> set;

    set.add(1, {10, 11});
    set.add(2, {20, 22});
    set.add(3, {30, 33});

    set.remove(2);

    CHECK(set.size() == 2);
    CHECK(!set.has(2));
    CHECK(set.get(2) == nullptr);

    CHECK(set.get(1)->x == 10);
    CHECK(set.get(3)->x == 30);
}

void removeLast() {
    vertex::SparseSet<Position> set;

    set.add(1, {10, 11});
    set.add(2, {20, 22});

    set.remove(2);

    CHECK(set.size() == 1);
    CHECK(!set.has(2));
    CHECK(set.get(1)->x == 10);
}

void removeEverything() {
    vertex::SparseSet<Position> set;

    set.add(1, {10, 11});
    set.add(2, {20, 22});
    set.add(3, {30, 33});

    set.remove(1);
    set.remove(3);
    set.remove(2);

    CHECK(set.size() == 0);
    CHECK(!set.has(1) && !set.has(2) && !set.has(3));
}

void removeMissingIsHarmless() {
    vertex::SparseSet<Position> set;
    set.add(1, {10, 11});

    set.remove(0);
    set.remove(99);
    set.remove(1);
    set.remove(1);

    CHECK(set.size() == 0);
}

void addTwiceOverwrites() {
    vertex::SparseSet<Position> set;

    set.add(1, {10, 11});
    set.add(1, {99, 98});

    CHECK(set.size() == 1);
    CHECK(set.get(1)->x == 99);
}

void sparseGrowsWithoutFalsePositives() {
    vertex::SparseSet<Position> set;
    set.add(5000, {1, 2});

    for (std::uint32_t id = 0; id < 5000; ++id) {
        CHECK(!set.has(id));
        CHECK(set.get(id) == nullptr);
    }

    CHECK(set.has(5000));
    CHECK(set.get(5000)->x == 1);
    CHECK(set.size() == 1);
}

void ownersStayInSyncAfterManyRemovals() {
    vertex::SparseSet<Position> set;

    for (std::uint32_t id = 0; id < 100; ++id) {
        set.add(id, {static_cast<float>(id), 0});
    }

    for (std::uint32_t id = 0; id < 100; id += 2) {
        set.remove(id);
    }

    CHECK(set.size() == 50);

    for (std::uint32_t id = 0; id < 100; ++id) {
        if (id % 2 == 0) {
            CHECK(!set.has(id));
        } else {
            CHECK(set.has(id));
            CHECK(set.get(id)->x == static_cast<float>(id));
        }
    }

    const auto& owners = set.owners();
    for (std::size_t slot = 0; slot < owners.size(); ++slot) {
        CHECK(set.get(owners[slot])->x == static_cast<float>(owners[slot]));
    }
}

void movableComponentsAreNotCorrupted() {
    vertex::SparseSet<std::string> set;

    set.add(1, "one");
    set.add(2, "two");
    set.add(3, "three");

    set.remove(1);

    CHECK(set.size() == 2);
    CHECK(*set.get(2) == "two");
    CHECK(*set.get(3) == "three");
}

}

int main() {
    addAndGet();
    missingEntity();
    removeFromMiddle();
    removeLast();
    removeEverything();
    removeMissingIsHarmless();
    addTwiceOverwrites();
    sparseGrowsWithoutFalsePositives();
    ownersStayInSyncAfterManyRemovals();
    movableComponentsAreNotCorrupted();

    std::printf("SparseSet: all tests passed\n");
    return 0;
}
