#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>

namespace vertex::tests {

class QuadTree {
public:
    // Max items in a leaf before it splits.
    static constexpr int kCapacity = 8;
    
    // Safety cap only; splitting normally stops at minNodeSize 
    static constexpr int kMaxDepth = 16;

    // Makes the tree a single empty root covering [origin, origin + extent]. A node
    // only splits if its children would be at least minNodeSize on each side.
    void reset(const glm::vec2& origin, const glm::vec2& extent, float minNodeSize);

    void insert(std::uint32_t id, const glm::vec2& position, const glm::vec2& size);

    void query(const glm::vec2& position, const glm::vec2& size,
               std::vector<std::uint32_t>& out) const;

    // Nodes in use (root, branches, and leaves), for inspecting how the tree split.
    int nodeCount() const {
        return mNodeCount;
    }

private:
    struct Item {
        std::uint32_t id;
        glm::vec2 min;
        glm::vec2 max;
    };

    struct Node {
        glm::vec2 min{0.0f, 0.0f};
        glm::vec2 max{0.0f, 0.0f};
        int depth{0};
        // Index of the first of four consecutive children in mNodes, or -1 for a leaf.
        int firstChild{-1};
        // Only leaves hold items; a node's items move to its children when it splits.
        std::vector<Item> items;
    };


    int allocateNode(const glm::vec2& min, const glm::vec2& max, int depth);
    bool canSplit(const Node& node) const;
    void insertInto(int nodeIndex, const Item& item);
    void split(int nodeIndex);
    void collect(int nodeIndex, const glm::vec2& min, const glm::vec2& max,
                 std::vector<std::uint32_t>& out) const;

    // Nodes are linked by index
    std::vector<Node> mNodes;
    int mNodeCount{0};
    float mMinNodeSize{1.0f};
    // Holds a splitting node's items while they're re-inserted into its children, one
    // buffer per depth.
    std::vector<std::vector<Item>> mSplitBuffers =
        std::vector<std::vector<Item>>(kMaxDepth + 1);
};

}
