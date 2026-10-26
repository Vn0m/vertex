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

    // world position of the top left corner of the grid (set in reset)
    glm::vec2 mOrigin{0.0f, 0.0f};

    // Side lenght of the squares, function GridCollision sets this to double the
    // largest side
    float mCellSize{1.0f};

    // number of columns
    int mColumns{0};

    // number of rows
    int mRows{0};

    // mCells[row * mColumns + column] holds the ids listed in that cell.
    // will be treated as a flat array
    // outer vector indicates cell, inner indicated ID for the objects
    std::vector<std::vector<std::uint32_t>> mCells;

    // index = row * mColumns + column
    // mColumn = 4, row 2 : cell(2,1) = 2 * 4 + 1 = 9
    //         col0 col1 col2 col3
    // row0 [   0    1    2    3 ]
    // row1 [   4    5    6    7 ]
    // row2 [   8    9   10   11 ]
};

}
