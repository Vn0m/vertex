#pragma once

#include <cstddef>
#include <cstdint>
#include <memory>
#include <typeindex>
#include <unordered_map>
#include <vector>

#include "vertex/Entity.h"
#include "vertex/SparseSet.h"

namespace vertex {

class Registry {
public:
    Entity create() {
        if (!mFreeIds.empty()) {
            const std::uint32_t index = mFreeIds.back();
            mFreeIds.pop_back();
            mAlive[index] = 1;
            return {index, mGenerations[index]};
        }

        mGenerations.push_back(0);
        mAlive.push_back(1);
        return {static_cast<std::uint32_t>(mGenerations.size() - 1), 0};
    }

    void destroy(Entity entity) {
        if (!valid(entity)) {
            return;
        }

        for (auto& entry : mPools) {
            entry.second->remove(entity.index);
        }

        mAlive[entity.index] = 0;
        mGenerations[entity.index]++;
        mFreeIds.push_back(entity.index);
    }

    bool valid(Entity entity) const {
        return entity.index < mGenerations.size() && mAlive[entity.index] != 0 &&
               mGenerations[entity.index] == entity.generation;
    }

    std::size_t aliveCount() const {
        return mGenerations.size() - mFreeIds.size();
    }

    template <typename T>
    T* add(Entity entity, T component) {
        if (!valid(entity)) {
            return nullptr;
        }
        return &pool<T>().add(entity.index, std::move(component));
    }

    template <typename T>
    void remove(Entity entity) {
        if (!valid(entity)) {
            return;
        }
        pool<T>().remove(entity.index);
    }

    template <typename T>
    bool has(Entity entity) const {
        if (!valid(entity)) {
            return false;
        }
        const SparseSet<T>* set = findPool<T>();
        return set != nullptr && set->has(entity.index);
    }

    template <typename T>
    T* get(Entity entity) {
        if (!valid(entity)) {
            return nullptr;
        }
        return pool<T>().get(entity.index);
    }

    template <typename T>
    const T* get(Entity entity) const {
        if (!valid(entity)) {
            return nullptr;
        }
        const SparseSet<T>* set = findPool<T>();
        return set != nullptr ? set->get(entity.index) : nullptr;
    }

    template <typename A, typename Func>
    void each(Func fn) {
        SparseSet<A>& setA = pool<A>();
        const std::vector<std::uint32_t> owners = setA.owners();

        for (std::uint32_t id : owners) {
            fn(Entity{id, mGenerations[id]}, *setA.get(id));
        }
    }

    template <typename A, typename B, typename Func>
    void each(Func fn) {
        SparseSet<A>& setA = pool<A>();
        SparseSet<B>& setB = pool<B>();

        const bool aIsSmaller = setA.size() <= setB.size();
        const std::vector<std::uint32_t> owners =
            aIsSmaller ? setA.owners() : setB.owners();

        for (std::uint32_t id : owners) {
            A* a = setA.get(id);
            B* b = setB.get(id);
            if (a == nullptr || b == nullptr) {
                continue;
            }
            fn(Entity{id, mGenerations[id]}, *a, *b);
        }
    }

private:
    template <typename T>
    SparseSet<T>& pool() {
        const std::type_index key{typeid(T)};

        auto it = mPools.find(key);
        if (it == mPools.end()) {
            it = mPools.emplace(key, std::make_unique<SparseSet<T>>()).first;
        }

        return *static_cast<SparseSet<T>*>(it->second.get());
    }

    template <typename T>
    const SparseSet<T>* findPool() const {
        const auto it = mPools.find(std::type_index{typeid(T)});
        if (it == mPools.end()) {
            return nullptr;
        }
        return static_cast<const SparseSet<T>*>(it->second.get());
    }

    std::vector<std::uint32_t> mGenerations;
    std::vector<std::uint8_t> mAlive;
    std::vector<std::uint32_t> mFreeIds;
    std::unordered_map<std::type_index, std::unique_ptr<ISparseSet>> mPools;
};

}
