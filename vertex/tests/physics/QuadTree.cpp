#include "QuadTree.h"

#include <algorithm>

namespace vertex::tests {

namespace {

bool overlaps(const glm::vec2& aMin, const glm::vec2& aMax, const glm::vec2& bMin,
              const glm::vec2& bMax) {
    return aMin.x <= bMax.x && bMin.x <= aMax.x && aMin.y <= bMax.y && bMin.y <= aMax.y;
}

}

void QuadTree::reset(const glm::vec2& origin, const glm::vec2& extent,
                     float minNodeSize) {
    mMinNodeSize = minNodeSize;
    mNodeCount = 0;
    allocateNode(origin, origin + extent, 0);
}

void QuadTree::insert(std::uint32_t id, const glm::vec2& position,
                      const glm::vec2& size) {
    insertInto(0, {id, position, position + size});
}

void QuadTree::query(const glm::vec2& position, const glm::vec2& size,
                     std::vector<std::uint32_t>& out) const {
    out.clear();
    if (mNodeCount > 0) {
        collect(0, position, position + size, out);
    }

    // An item straddling several of the reached leaves was collected once per leaf.
    std::sort(out.begin(), out.end());
    out.erase(std::unique(out.begin(), out.end()), out.end());
}

int QuadTree::allocateNode(const glm::vec2& min, const glm::vec2& max, int depth) {
    if (mNodeCount == static_cast<int>(mNodes.size())) {
        mNodes.emplace_back();
    }

    Node& node = mNodes[static_cast<std::size_t>(mNodeCount)];
    node.min = min;
    node.max = max;
    node.depth = depth;
    node.firstChild = -1;
    node.items.clear();  // keeps capacity from earlier frames
    return mNodeCount++;
}

bool QuadTree::canSplit(const Node& node) const {
    const glm::vec2 childSize = (node.max - node.min) * 0.5f;
    return node.depth < kMaxDepth && childSize.x >= mMinNodeSize &&
           childSize.y >= mMinNodeSize;
}

void QuadTree::insertInto(int nodeIndex, const Item& item) {
    // Re-fetch nodes by index after anything that can allocate; references into mNodes
    // would dangle if it reallocated.
    if (mNodes[static_cast<std::size_t>(nodeIndex)].firstChild == -1) {
        Node& leaf = mNodes[static_cast<std::size_t>(nodeIndex)];
        leaf.items.push_back(item);
        if (static_cast<int>(leaf.items.size()) > kCapacity && canSplit(leaf)) {
            split(nodeIndex);
        }
        return;
    }

    const int firstChild = mNodes[static_cast<std::size_t>(nodeIndex)].firstChild;
    for (int child = firstChild; child < firstChild + 4; ++child) {
        const Node& node = mNodes[static_cast<std::size_t>(child)];
        if (overlaps(item.min, item.max, node.min, node.max)) {
            insertInto(child, item);
        }
    }
}

void QuadTree::split(int nodeIndex) {
    const glm::vec2 min = mNodes[static_cast<std::size_t>(nodeIndex)].min;
    const glm::vec2 max = mNodes[static_cast<std::size_t>(nodeIndex)].max;
    const glm::vec2 mid = (min + max) * 0.5f;
    const int depth = mNodes[static_cast<std::size_t>(nodeIndex)].depth;
    const int childDepth = depth + 1;

    // Children are allocated consecutively: top-left, top-right, bottom-left,
    // bottom-right.
    const int firstChild = allocateNode(min, mid, childDepth);
    allocateNode({mid.x, min.y}, {max.x, mid.y}, childDepth);
    allocateNode({min.x, mid.y}, {mid.x, max.y}, childDepth);
    allocateNode(mid, max, childDepth);

    // Swap 
    std::vector<Item>& buffer = mSplitBuffers[static_cast<std::size_t>(depth)];
    Node& node = mNodes[static_cast<std::size_t>(nodeIndex)];
    node.firstChild = firstChild;
    buffer.swap(node.items);

    // node now has children
    for (const Item& item : buffer) {
        insertInto(nodeIndex, item);
    }
    buffer.clear();
}

void QuadTree::collect(int nodeIndex, const glm::vec2& min, const glm::vec2& max,
                       std::vector<std::uint32_t>& out) const {
    const Node& node = mNodes[static_cast<std::size_t>(nodeIndex)];
    if (!overlaps(min, max, node.min, node.max)) {
        return;
    }

    if (node.firstChild == -1) {
        for (const Item& item : node.items) {
            out.push_back(item.id);
        }
        return;
    }

    for (int child = node.firstChild; child < node.firstChild + 4; ++child) {
        collect(child, min, max, out);
    }
}

}
