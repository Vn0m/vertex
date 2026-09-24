#pragma once

#include <cstdint>
#include <vector>
namespace vertex {

class ISparseSet {
public:
    virtual ~ISparseSet() = default;
    virtual bool has(std::uint32_t id) const = 0;
    virtual void remove(std::uint32_t id) = 0;
};

template <typename T>
class SparseSet : public ISparseSet {
private:
    static constexpr std::uint32_t kNone = static_cast<std::uint32_t>(-1);
    std::vector<std::uint32_t> mSparse;
    std::vector<std::uint32_t> mOwners;
    std::vector<T> mDense;
public:
    bool has(std::uint32_t id) const override {
        if (id >= mSparse.size() || mSparse[id] == kNone) {
            return false;
        }
        return true;
    }

    T* get(std::uint32_t id) {
        return has(id) ? &mDense[mSparse[id]] : nullptr;
    }

    const T* get(std::uint32_t id) const {
        return has(id) ? &mDense[mSparse[id]] : nullptr;
    }
};
}