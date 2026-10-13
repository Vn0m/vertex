#pragma once

#include <cstdint>
#include <vector>

#include <glm/vec2.hpp>

namespace vertex::tests {

class SpatialGrid {
public:
    void reset(const glm::vec2& origin, const glm::vec2& extent, float cellSize);

    void insert(std::uint32_t id, const glm::vec2& position, const glm::vec2& size);

    void query(const glm::vec2& position, const glm::vec2& size,
               std::vector<std::uint32_t>& out) const;

    int columns() const {
        return mColumns;
    }
    int rows() const {
        return mRows;
    }

private:
    struct CellRange {
        int minColumn;
        int minRow;
        int maxColumn;
        int maxRow;
    };

    CellRange cellRange(const glm::vec2& position, const glm::vec2& size) const;

    glm::vec2 mOrigin{0.0f, 0.0f};
    float mCellSize{1.0f};
    int mColumns{0};
    int mRows{0};
    // mCells[row * mColumns + column] holds the ids listed in that cell.
    std::vector<std::vector<std::uint32_t>> mCells;
};

}
