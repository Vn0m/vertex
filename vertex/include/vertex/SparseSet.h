#pragma once

#include <cstddef>
#include <cstdint>
#include <utility>
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

    T& add(std::uint32_t id, T component) {
        if (id >= mSparse.size()) {
            mSparse.resize(id + 1, kNone);
        }

        if (mSparse[id] != kNone) {
            mDense[mSparse[id]] = std::move(component);
            return mDense[mSparse[id]];
        }

        mDense.push_back(std::move(component));
        mOwners.push_back(id);
        mSparse[id] = static_cast<std::uint32_t>(mDense.size() - 1);
        return mDense.back();
    }

    void remove(std::uint32_t id) override {
        if (!has(id)) {
            return;
        }

        const std::uint32_t slot = mSparse[id];
        const std::uint32_t last = static_cast<std::uint32_t>(mDense.size() - 1);

        if (slot != last) {
            mDense[slot] = std::move(mDense[last]);
            mOwners[slot] = mOwners[last];
            mSparse[mOwners[slot]] = slot;
        }

        mDense.pop_back();
        mOwners.pop_back();
        mSparse[id] = kNone;
    }

    std::size_t size() const {
        return mDense.size();
    }

    std::vector<T>& components() {
        return mDense;
    }

    const std::vector<std::uint32_t>& owners() const {
        return mOwners;
    }
};
}
