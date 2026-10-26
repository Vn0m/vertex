#include "physics/SpatialGrid.h"

#include <algorithm>
#include <cmath>

namespace vertex::tests {

namespace {

// find the row and cell count.
int cellsAlong(float length, float cellSize) {
    return std::max(static_cast<int>(std::ceil(length / cellSize)), 1);
}

int clampedCell(float offset, float cellSize, int cellCount) {
    const int cell = static_cast<int>(std::floor(offset / cellSize));
    return std::clamp(cell, 0, cellCount - 1);
}

}

// @brief find the origin of the grid, cell size,
// @param origin: coordinates of top left corner cell(0,0) received these coordinates
// @param extent: width and height of the area to cover
// @param cellSize: 2 * largestSide
void SpatialGrid::reset(const glm::vec2& origin, const glm::vec2& extent,
                        float cellSize) {
    mOrigin = origin;
    mCellSize = cellSize;

    // find number of columns and based on the dimension of the window / cellsize
    mColumns = cellsAlong(extent.x, cellSize);
    mRows = cellsAlong(extent.y, cellSize);

    mCells.resize(static_cast<std::size_t>(mColumns) * static_cast<std::size_t>(mRows));
    for (auto& cell : mCells) {
        // clear out the objects in each cell.
        cell.clear();
    }
}

// @brief record one body in the grid, and record every cell this body overlaps
// then each cell holds the body in it
// @param id of the object
// @param position of the object
// @param size: height and width of the object
void SpatialGrid::insert(std::uint32_t id, const glm::vec2& position,
                         const glm::vec2& size) {
    // convert the space position to a range of grid cells
    const CellRange range = cellRange(position, size);

    // traverse the range of rows and columns a overlaps and allocate id in the
    // corresponding cell
    // loop runs at most 4 times.
    for (int row = range.minRow; row <= range.maxRow; ++row) {
        for (int column = range.minColumn; column <= range.maxColumn; ++column) {
            // flattened array index allocation
            mCells[static_cast<std::size_t>(row * mColumns + column)].push_back(id);
        }
    }
}

// @brief given a position (box), return all of the ids, in that cell.
// @param position of the object
// @param size of the body
// @param reference to a vector of ID, this is what this function modifies.
void SpatialGrid::query(const glm::vec2& position, const glm::vec2& size,
                        std::vector<std::uint32_t>& out) const {
    // clear out the vector from previous call.
    out.clear();

    // find range of rows and cols for the object
    const CellRange range = cellRange(position, size);
    for (int row = range.minRow; row <= range.maxRow; ++row) {
        for (int column = range.minColumn; column <= range.maxColumn; ++column) {
            // get the vector containing ids from a corresponding cells
            const auto& cell = mCells[static_cast<std::size_t>(row * mColumns + column)];

            // append to the end of "neightbors"
            out.insert(out.end(), cell.begin(), cell.end());
        }
    }

    // An item spanning several of the queried cells was collected once per cell.
    // sort ids
    std::sort(out.begin(), out.end());

    // move duplicates to the end and return iterator with new end.
    out.erase(std::unique(out.begin(), out.end()), out.end());
}

SpatialGrid::CellRange SpatialGrid::cellRange(const glm::vec2& position,
                                              const glm::vec2& size) const {
    // top left corner relative to the origin
    const glm::vec2 minOffset = position - mOrigin;

    // bottom right relative to the origin
    const glm::vec2 maxOffset = position + size - mOrigin;

    // determine the range for the object
    return {clampedCell(minOffset.x, mCellSize, mColumns),
            clampedCell(minOffset.y, mCellSize, mRows),
            clampedCell(maxOffset.x, mCellSize, mColumns),
            clampedCell(maxOffset.y, mCellSize, mRows)};
}

}
